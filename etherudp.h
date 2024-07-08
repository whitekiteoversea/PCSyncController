#ifndef ETHERUDP_H
#define ETHERUDP_H

#include <QObject>
#include <QUdpSocket>
#include <QObject>
#include <QByteArray>
#include <QAxObject>
#include <QVariant>
#include "FrameSheet.h"
//#include "wirelessserialport.h"

class Etherudp : public QObject
{
    Q_OBJECT
public:
    explicit Etherudp(QObject *parent = nullptr);
    ~Etherudp();

signals:
    void RecvToRefreshPage(); //通知界面更新数据
    void UpdateSpeed(speedUpdateFormat UpdatedInfo);
    void UpdateSpeed_2(speedUpdateFormat UpdateInfo);
    void timeSynDeal(unsigned char BoradNum, unsigned int recvtime, unsigned int replyTime);
    void readyForRecv(unsigned short len); //准备好传输数据
    void storageOk(unsigned short recvLen); //告知传输结束

    void updateSystemModel(unsigned char sendNo, feedbackData sampleData); //接收到最新的x_estimate，通知更新预测控制序列u_arr
    void updateStatus(unsigned char sendNo, CASREPORTFRAME statusData);
    void updateRTTDelay(unsigned char sendNo, feedbackData sampleData); //更新同步RTT时延
    void updateSDRAMData(unsigned char sendNo, unsigned int currentSubPackNum, unsigned int totalPackNum, unsigned int writeNum, SUBPACK* array); //更新SDRAM数据
    void readyToSDRAMTrans(unsigned char sendNo);
public slots:
    void read_data(); //解析通信内容
    void closeObjectSlot(); //终止通信对象

private:
    volatile bool UDPThreadEnd; //线程结束
    QUdpSocket *mSocket;
};

//本次待发送数据条数记录
extern uint8_t curSndCANMsgCnt;


//速度时间戳记录表
extern QList<unsigned int> timeStamplist_1;
extern QList<unsigned short> speedlist_1;

//大小端转换
uint16_t word_trans(uint16_t number);
uint32_t Dword_trans(uint32_t number);
uint64_t DDword_trans(uint64_t number);

#endif // ETHERUDP_H
