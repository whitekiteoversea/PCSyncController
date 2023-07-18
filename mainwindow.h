#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include <wirelessserialport.h>
#include <QTimer>
#include <QUdpSocket>
#include <QFileDialog>
#include <QAxObject>
#include <QDir>
#include <QTime>
#include "FrameSheet.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void plotParaSetup(); //绘图设置
    qint64 packetSend(unsigned char sendNo, unsigned char NodeCmd, unsigned char *data);   //以太网报文发送
    void initStyle(); //样式表加载

    //下发以太网报文
    void sendRequestSig();          //20ms周期通信信号发送
    void sendTimeSyncSig();         //10ms周期时间同步信号发送
    void sendGivenSpeedSig(unsigned char sendNo, short givenSpeed);       //发送单板速度给定
    void sendGivenPreSpeedSig(unsigned char sendNo, feedbackData sampleData, uint8_t prelen);    //发送单板预测给定

    //输出反馈预测
    EthUARRPRE preControllerUpdateY();

    // NPC离散预测控制器(生成预测u_arr)
    EthUARRPRE preControllerUpdate(uint32_t curTimeStamp, double *x_esti_arr, unsigned char prelen);

    // 同步误差消除控制器
    short SMCpreController();
    short pdGravitypreController();

    // 文件存储
    char* GetCuurentFilePath();

signals:
    void timerCTRSend(unsigned char presetTimerStatus); //发送定时器状态变更命令
    void ethernetCloseCTRSend();                        //结束以太网接收线程
    void sendRefSineSig(int sendCnt);                   //发送正弦参考信号

private slots:
    void onTimeout(unsigned int recvCurTimeStamp_Ms); //超时打印时间戳
    void on_timerSet_clicked();              //定时器开启/关闭
    void pageMsgRefresh();                   //界面信息更新
    void realtimeDataSlot();                 //定时器调用，速度显示刷新

    void on_synStop_clicked();               //同步停车
    void on_pushButton_4_clicked();          //扫描可用串口
    void on_pushButton_2_clicked();          //连接指定串口电机1并加锁
    void on_pushButton_3_clicked();          //连接指定串口电机2并加锁

    void serial1DataRefresh(speedUpdateFormat updatePack); //串口1数据更新
    void serial2DataRefresh(speedUpdateFormat updatePack); //串口2数据更新

    void upreupdateJudge(unsigned char sendNo, feedbackData sampleData);  //判断C-A通道是否需要更新u_arr
    void rttDelayUpdate(unsigned char sendNo, feedbackData sampleData);  //RTT更新
    void updateAvgPosi(short avgPosi); //平均同步位置信息更新

    //以太网报文更新数据
    void UpdateFeedbackSpeed1rpm(speedUpdateFormat curSpeedTime);
    void UpdateFeedbackSpeed2rpm(speedUpdateFormat curSpeedTime);

    void on_pushButton_5_clicked();
    void on_synStart_clicked();
    void on_speedGiven_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_6_clicked();
    void on_PosiLoopInit_clicked();
    void on_StorePMSM2_clicked();
    void on_StorePMSM1_clicked();
    void on_StorePMSM3_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *dataTimer;
    QLabel *statusLabel;
    WirelessSerialPort *speedCurTask_1;
    WirelessSerialPort *speedCurTask_2;
    QCPTextElement *m_title;
    QUdpSocket *sendSocket;  //发送Socket
    QAxObject *excel;
    QTime currentTime;
};

//系统状态方程一步迭代
typedef struct {
    double x_pre[3];
    double u_pre;
}SYSPREARR;


uint16_t Delay_xms(uint16_t value); //延时函数
SYSPREARR oneStepPre(double *x_esti_arr);//一步预测
short AverageSpeedCal(uint32_t recvPulse);
//DPETC 判断C-A通道是否触发通信事件
uint8_t dpetc_U_update(unsigned char sendNo, feedbackData sampleData);

short* posiRefCal(unsigned int dstPosiIncre); //根据目标位置增量生成速度参考曲线
int satFunc(int inputS);

#endif // MAINWINDOW_H
