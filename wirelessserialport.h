#ifndef WIRELESSSERIALPORT_H
#define WIRELESSSERIALPORT_H

#include <QSerialPort>
#include <QObject>
#include <QSerialPortInfo>
#include <QObject>
#include <QMutex>
#include <QList>
#include "FrameSheet.h"

class WirelessSerialPort : public QObject
{
    Q_OBJECT

public:
    explicit WirelessSerialPort(QObject *parent = nullptr);
    ~WirelessSerialPort();

    Q_INVOKABLE QStringList getAvailablePorts(void); //获取可用端口
    Q_INVOKABLE bool openPort(const QString &portName, qint32 portBaudRate); //打开端口
    Q_INVOKABLE void closePort(void);   //关闭端口

public slots:
    void readyReadPort(void);           //串口数据读取
    void analyseProtocol(char &data);   //串口协议分析

signals:
    void sendToSpeedGraph(speedUpdateFormat pack);            //发送串口数据到UI绘图


private:
    QSerialPort *mSerialPort;
    bool isRec = true;                  //处于串口接收状态
};

extern QMutex m_Mutex; //加锁

#endif // WIRELESSSERIALPORT_H

