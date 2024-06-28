#include "etherudp.h"
#include <QMessageBox>
#include <QDebug>
#include <QHostAddress>
#include <QString>
#include "FrameSheet.h"
#include <QNetworkProxy>
#include <QFileDialog>
#include <QVariant>

#define RECVLEN  (600)  // 接收区长度
#define SENDLEN  (600)  // 接收区长度

// ETH-CAS Mode
QString bindIP = "192.168.20.33"; //上位机接收IP
uint16_t bindPort = 8888;  //上位机接收端口

uint8_t etherRecvBuf[RECVLEN] = {0}; //以太网接收缓冲区
uint8_t curSndCANMsgCnt = 0; //记录本次发送的CAN消息条数
uint8_t etherudpSndBuf[SENDLEN] = {0};  //以太网报文发送缓冲区
uint32_t recvTimeSyn[2] = {0};

//动态分配接收大量数据
QList<unsigned int> timeStamplist_1;
QList<unsigned short> speedlist_1;
unsigned int recvTotalCnt = 0;

Etherudp::Etherudp(QObject *parent) : QObject(parent)
{
    uint8_t rtConnStatus = false;

    // 创建UDP对象并初始化
    mSocket = new QUdpSocket(this);

    // 关闭网络代理
    //QNetworkProxyFactory::setApplicationProxy(false);

    // 绑定接收端口及IP，Windows下允许重复绑定
    if(false == mSocket->bind(QHostAddress(bindIP), bindPort, QAbstractSocket::ShareAddress)) {
        QMessageBox::critical(0, "警告！", "UDP接收IP及端口绑定失败！请检查本机IP及配置文件...",QMessageBox::Cancel);
        UDPThreadEnd = true;
    } else { //正常启动通信线程
        UDPThreadEnd = false;
    }

    // Extern -> Recv
    connect(mSocket,SIGNAL(readyRead()),this,SLOT(read_data()),Qt::AutoConnection);
}

Etherudp::~Etherudp()
{

}

//读取以太网协议包
void Etherudp::read_data()
{
    QByteArray ba;
    uint32_t tempPosi = 0x0000;
    int32_t tmpSyncErr = 0;
    feedbackData sampleData;
    EthControlFrame recvFeedbackPack;

    CASREPORTFRAME recvCASFrame;
    uint8_t cnt =0;

    /*接收以太网数据包,注意处理多字节数据大小端问题*/
    while(mSocket->hasPendingDatagrams())
    {
        //1、接收线程开启判断
        if(UDPThreadEnd == true) {
            break;
        }
        //2、接收准备
        ba.resize(mSocket->pendingDatagramSize());
        mSocket->readDatagram(ba.data(),ba.size());

        // CAS Mode
        if (curAlgoMode == 0) {
            memset(&etherRecvBuf, 0, sizeof(etherRecvBuf));
            memcpy(&etherRecvBuf,ba.data(),ba.size());
            memset(&recvCASFrame, 0, sizeof(recvCASFrame));
            memcpy(&recvCASFrame, etherRecvBuf, sizeof(recvCASFrame));

            switch (recvCASFrame.EType) {
                case CANPosiAcquireCmd:  //状态反馈
                    emit updateStatus(recvCASFrame.CASNodeID, recvCASFrame);
                    qDebug() << "FeedbackPosi: " + QString::number(recvCASFrame.motorPosiUM ,10) + "um \n";
                break;

                case CANDriverInfoAcquire:  //SDRAM数据上传

                break;

                default:
                    break;
            }
        } else if (curAlgoMode == 1) {
            //3、准备接收区
            memset(&etherRecvBuf, 0, sizeof(etherRecvBuf));
            memcpy(&etherRecvBuf,ba.data(),ba.size());
            memset(&recvFeedbackPack, 0, sizeof(recvFeedbackPack));
            memcpy(&recvFeedbackPack, etherRecvBuf, sizeof(recvFeedbackPack));
            //根据报文类型解包(接收的报文结构体不一致)
            for (cnt = 0; cnt<3; cnt++) {
                if (recvFeedbackPack.canEnable[cnt] == 1) {
                    switch(recvFeedbackPack.canCmd[cnt].CANID.STDCANID.CTRCode) {
                    case 4:
                        sampleData.frameNum = recvFeedbackPack.canCmd[cnt].CANData[0];
                        sampleData.frameNum <<= 8;
                        sampleData.frameNum |= recvFeedbackPack.canCmd[cnt].CANData[1];

                        sampleData.sampleTimeStamp = recvFeedbackPack.canCmd[cnt].CANData[2];
                        sampleData.sampleTimeStamp <<= 8;
                        sampleData.sampleTimeStamp |= recvFeedbackPack.canCmd[cnt].CANData[3];
                        sampleData.sampleTimeStamp <<= 8;
                        sampleData.sampleTimeStamp |= recvFeedbackPack.canCmd[cnt].CANData[4];

                        emit updateRTTDelay(cnt+1, sampleData);
                        break;

                    case 5:
                        sampleData.frameNum = recvFeedbackPack.canCmd[cnt].CANData[0];
                        sampleData.frameNum <<= 8;
                        sampleData.frameNum |= recvFeedbackPack.canCmd[cnt].CANData[1];

                        tempPosi= recvFeedbackPack.canCmd[cnt].CANData[5];
                        tempPosi <<= 8;
                        tempPosi |= recvFeedbackPack.canCmd[cnt].CANData[6];
                        tempPosi <<= 8;
                        tempPosi |= recvFeedbackPack.canCmd[cnt].CANData[7];
                        sampleData.feedbackPosium = (int)tempPosi;
                        emit updateSystemModel(cnt+1, sampleData); // 更新离散模型
                        break;

                    case 6:
                        sampleData.frameNum = recvFeedbackPack.canCmd[0].CANData[0];
                        sampleData.frameNum <<= 8;
                        sampleData.frameNum |= recvFeedbackPack.canCmd[0].CANData[1];

                        tmpSyncErr= recvFeedbackPack.canCmd[0].CANData[5];
                        tmpSyncErr <<= 8;
                        tmpSyncErr |= recvFeedbackPack.canCmd[0].CANData[6];
                        tmpSyncErr <<= 8;
                        tmpSyncErr |= recvFeedbackPack.canCmd[0].CANData[7];
                        // emit updateRealTimePosi(tmpSyncErr); // 更新实时同步误差
                        break;

                    default:
                        break;
                    }
                }
            }
        }
    }
}

//终止UDP接收
void Etherudp::closeObjectSlot()
{
    UDPThreadEnd = true;
}

//拼接Word
uint16_t WordCombine(uint8_t byte1, uint8_t byte2)
{
    uint16_t RecvWord;
    RecvWord = byte1;
    RecvWord <<= 8;
    RecvWord |= byte2;
    return RecvWord;
}

uint32_t Dword_trans(uint32_t number)
{
    uint32_t trans = 0;
    trans = (((number & 0x000000ff) <<24) | ((number & 0x00ff00) << 8)|
             ((number & 0x00ff0000)>>8) | ((number & 0xff000000)) >>24);
    return trans;
}

//uint16 大小端转换
uint16_t word_trans(uint16_t number)
{
    uint16_t trans = 0;
    trans = ((number & 0x00ff) << 8 | (number & 0xff00) >> 8);
    return trans;
}

//uint64 大小端转换
uint64_t DDword_trans(uint64_t number)
{
    uint64_t trans = 0;
    trans = (((number & 0x00000000000000ff)  << 56) | ((number & 0x000000000000ff00) << 40)|
             ((number & 0x0000000000ff0000)  << 24) | ((number & 0x00000000ff000000) << 8) |
             ((number & 0x00000000ff000000)  >> 8)  | ((number & 0x000000ff00000000) >> 24)|
             ((number & 0x00ff000000000000)  >> 40) | ((number & 0xff00000000000000) >> 56));
    return trans;
}
