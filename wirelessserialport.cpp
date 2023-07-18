#include "wirelessserialport.h"
#include "FrameSheet.h"
#include <QFile>
#include <QTextStream>
#include <QAxObject>
#include <QDebug>

QMutex m_Mutex;     //加锁以避免重复响应
speedUpdateFormat serialRecvBuf[BufMaxStorageCnt];
volatile int recvCnt = 0;
volatile int serialInt = 0;

WirelessSerialPort::WirelessSerialPort(QObject *parent)
{
    mSerialPort = new QSerialPort(this);
    //创建存储excel

    connect(mSerialPort, &QSerialPort::readyRead, this, &WirelessSerialPort::readyReadPort);
}

WirelessSerialPort::~WirelessSerialPort()
{

}

//串口参数设置
bool WirelessSerialPort::openPort(const QString &portName, qint32 portBaudRate)
{
    //串口协议帧格式为 1位起始 8位数据 无校验 1个停止位 无流控
    mSerialPort->setPortName(portName);
    mSerialPort->setBaudRate(portBaudRate);
    mSerialPort->setDataBits(QSerialPort::Data8);
    mSerialPort->setParity(QSerialPort::NoParity);
    mSerialPort->setStopBits(QSerialPort::OneStop);
    mSerialPort->setFlowControl(QSerialPort::NoFlowControl);
    //启动
    return mSerialPort->open(QIODevice::ReadWrite);
}

//获取可选择的串口对象(这里需要进一步处理以适应串口动态插拔的情况)
QStringList WirelessSerialPort::getAvailablePorts(void)
{
    static QStringList serialPortNameList; //获取到的串口名
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if (serial.open(QIODevice::ReadWrite))
        {
            serialPortNameList.append(serial.portName());
            serial.close();
        }
    }
    return serialPortNameList;
}
//关闭串口
void WirelessSerialPort::closePort(void)
{
    mSerialPort->waitForBytesWritten();
    mSerialPort->close();
}

//串口接收协议解析
void WirelessSerialPort::analyseProtocol(char &data)
{
    //获取信息
    static unsigned char logBuffer[256];
    static unsigned char *pData = (unsigned char *)&data;
    static unsigned char RecvStatus = 0;       //当前数据帧接收进度
    static unsigned char bufCnt = 0;
    static unsigned char frameDataLength = 0; //其实是用来计量已经接收到的数据长度
    static unsigned char sumCheck = 0;
    static unsigned char dataLenCnt = 0;

    unsigned int recvTimeStamp = 0;     //接收到的时间戳
    unsigned short recvSpeed = 0;       //得到的速度数据
    speedUpdateFormat speedInfoPack;    //待发送的包

    //根据通信协议进行串口数据解包
    //1、帧头 0xAA55
    if(*pData == 0xAA && RecvStatus == 0)
    {
        logBuffer[bufCnt++] = *pData;
        RecvStatus = 1;
    }
    else if(*pData == 0x55 && RecvStatus == 1)
    {
        logBuffer[bufCnt++] = *pData;
        RecvStatus = 2;
    }
    //2、信号来源：板卡1/2
    else if(RecvStatus == 2)
    {
        logBuffer[bufCnt++] = *pData;
        RecvStatus = 3;
    }
    //3、数据段长度
    else if(RecvStatus == 3)
    {
        logBuffer[bufCnt++] = *pData;
        frameDataLength =  *pData; //数据段长度
        RecvStatus = 4;
    }
    //4、数据段：100us时间戳、速度
    else if(RecvStatus == 4)
    {
        dataLenCnt++;
        logBuffer[bufCnt++] = *pData;
        if(dataLenCnt >= frameDataLength)
        {
            dataLenCnt = 0;
            RecvStatus = 5;
        }
    }
    //5、帧尾及拆包
    else if(RecvStatus == 5 && *pData == 0xBF)
    {
        logBuffer[bufCnt++] = *pData;

        //时间戳
        for(int i = 4; i<8; i++)
        {
            recvTimeStamp |= logBuffer[i];
            recvTimeStamp <<= ((7-i)*8);
        }

        //速度
        recvSpeed |= logBuffer[8];
        recvSpeed <<= 8;
        recvSpeed |= logBuffer[9];
        qDebug() << "recvSpeed " << recvSpeed;

        //本地缓存存储（对半存储）
        if(recvCnt >= SingleShotMaxStorageCnt-1)
        {
            recvCnt = 0;
        }
        serialRecvBuf[recvCnt].timeStamp = recvTimeStamp;
        serialRecvBuf[recvCnt].curSpeed = recvSpeed;
        recvCnt++;

        speedInfoPack.timeStamp = recvTimeStamp;
        speedInfoPack.curSpeed = recvSpeed;

        emit sendToSpeedGraph(speedInfoPack); //发送更新数据

        //还原接收初始状态
        RecvStatus = 0; //一帧接收成功， 重置接受状态
        sumCheck = 0;
        frameDataLength = 0;
        bufCnt = 0;
    }
    //数据无效
    else
    {
        RecvStatus = 0;
        sumCheck = 0;
        frameDataLength = 0;
        bufCnt = 0;
    }
}

//加锁并且将接收串口数据
void WirelessSerialPort::readyReadPort(void)
{
    if (m_Mutex.tryLock())
    {
        QByteArray data = mSerialPort->readAll();
        if (isRec)
        {
            foreach(char elem, data)
            {
                analyseProtocol(elem);
            }
        }
    }
    m_Mutex.unlock();
}
