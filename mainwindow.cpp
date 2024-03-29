#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <etherudp.h>
#include <QThreadPool>
#include <QTimer>
#include "timemonitor.h"
#include <QEventLoop>
#include <QSerialPortInfo>
#include <QStringList>
#include "FrameSheet.h"
#include <math.h>
#include <complex>
#include <QVariant>
#include <complex>
#include "pid.h"

// 功能开关
#define SerialPortEnable (0)    //串口默认关闭
#define TimeSyncEnable   (1)    //报文时钟同步默认开启
#define SpeedSyncMode    (0)    //测试用，三通道速度给定一致

// 数据存储相关
#define maxStorageLen 12000

// 帧号相关
#define initFrameNum_TimeSync 0
#define maxFrameNum_TimeSync 19999
#define initFrameNum_SpeedGiven 20000
#define maxFrameNum_SpeedGiven 39999
#define initFrameNum_SpeedPre 40000
#define maxFrameNum_SpeedPre 59999

// 编码器参数常数
#define PULSENUM 2060

//可用串口列表
QStringList *availbleSerialPort;

//工作模式
unsigned char workMode= 0;  // 0:默认模式;1:位置曲线控制;2:同步控制
short *refPosiArray = NULL; //获取
int posiRef = 0;            //位置PID控制给定

uint32_t timeStamp[maxStorageLen];      //存放对应帧的utc时间
uint32_t recvtTimeStamp[maxStorageLen]; //存放对应帧的utc时间

// 同步模式记录实时平均位移
avgPosiRecord recordArray[maxStorageLen];

//PID控制器
PIDController pid;

//通道数据接收
uint16_t recPosiCnt_CH1 = 0;
uint16_t recPosiCnt_CH2 = 0;
uint16_t recPosiCnt_CH3 = 0;
feedbackData laFData_CH1;
feedbackData laFData_CH2;
feedbackData laFData_CH3;
uint32_t g_RecvTimeMS_CH1[maxStorageLen];   //存储反馈位置数据时间
uint32_t g_RecvTimeMS_CH2[maxStorageLen];   //存储反馈位置数据时间
uint32_t g_RecvTimeMS_CH3[maxStorageLen];   //存储反馈位置数据时间
feedbackData fDataPosi_CH1[maxStorageLen];       //存储反馈位置数据
feedbackData fDataPosi_CH2[maxStorageLen];       //存储反馈位置数据
feedbackData fDataPosi_CH3[maxStorageLen];       //存储反馈位置数据

//时间同步报文帧号 0-19999
volatile unsigned short g_FrameNum_CH[3] = {0};
volatile unsigned short g_FrameNum_CH2 = 0;

//速度预测报文帧号 40000-59999
volatile unsigned short g_PreFrameNum_CH1 = 40000;
volatile unsigned short g_PreFrameNum_CH2 = 40000;

EthControlFrame sendUDPPack; //以太网报文头
curTime curTimeStamp;

volatile unsigned char timeSynFlag = 0; //当前时钟同步状态
volatile unsigned int lastSynSndTime = 0;

//显示用计数
volatile int dispCnt1 = 0;
volatile int dispCnt2 = 0;
volatile int dispCANCnt1 = 0;
volatile int dispCANCnt2 = 0;

volatile int serialRecvCount = 0;
volatile unsigned int curToMs = 0;
volatile unsigned int firstTimeMs = 0;
volatile unsigned int refreshCnt = 0;

//对端IP、Port
QString dst_IP = "192.168.1.30";
int dst_Port = 8001;

// 全局计时 1ms
volatile unsigned int globalSynTime_ms = 0;
volatile int requestPacketFlag = 0;   //发送标志

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //通知栏
    statusLabel = new QLabel();
    ui->statusbar->setStyleSheet(QString("QStatusBar::item{border: 0px}")); // 不显示label的边框
    ui->statusbar->setSizeGripEnabled(false); //不显示右边的大小控制点
    ui->statusbar->addWidget(statusLabel);

    //1、以太网接收线程
    QThread *etherRecvThread = new QThread();
    etherRecvThread->start();
    //2、以太网接收对象
    Etherudp *localEthRecvTask = new Etherudp();
    localEthRecvTask->moveToThread(etherRecvThread);

    //以太网发送用Socket，绑定发送端口
    sendSocket = new QUdpSocket(this);
    sendSocket->open(QIODevice::ReadWrite);
    sendSocket->bind(dst_Port);

    connect(this, SIGNAL(ethernetCloseCTRSend()),
            localEthRecvTask,SLOT(closeObjectSlot()),Qt::DirectConnection);  //停止接收
    connect(localEthRecvTask,SIGNAL(RecvToRefreshPage()),
            this,SLOT(pageMsgRefresh()),Qt::DirectConnection); //根据接收信息刷新界面状态

    //以太网端更新最新速度值(暂时不用)
    connect(localEthRecvTask,SIGNAL(UpdateSpeed(speedUpdateFormat)),
            this,SLOT(UpdateFeedbackSpeed1rpm(speedUpdateFormat)),Qt::AutoConnection);
    connect(localEthRecvTask,SIGNAL(UpdateSpeed_2(speedUpdateFormat)),
            this ,SLOT(UpdateFeedbackSpeed2rpm(speedUpdateFormat)),Qt::AutoConnection);


    //停止接收并销毁以太网线程
    connect(this, &MainWindow::destroyed, etherRecvThread, [=]()
    {
        //停止以太网接收
        emit ethernetCloseCTRSend();
        //等待接收停止
        Delay_xms(10);
        //发信号停止计时线程
        if (etherRecvThread->isRunning())
        {
            etherRecvThread->quit();
            etherRecvThread->wait();
            //销毁计时线程
            etherRecvThread->deleteLater();
        }
    });

    //预测模型更新 事件触发判断
    connect(localEthRecvTask, SIGNAL(updateSystemModel(unsigned char, feedbackData)),
            this, SLOT(upreupdateJudge(unsigned char, feedbackData)), Qt::AutoConnection);

    connect(localEthRecvTask, SIGNAL(updateRTTDelay(unsigned char, feedbackData)),
            this, SLOT(rttDelayUpdate(unsigned char, feedbackData)), Qt::AutoConnection);

    connect(localEthRecvTask, SIGNAL(updateAvgPosi(short avgPosi)),
            this, SLOT(updateAvgPosi(short avgPosi)),Qt::AutoConnection);

#if SerialPortEnable
    //新建串口接收对象
    speedCurTask_1 = new WirelessSerialPort(this);
    speedCurTask_2 = new WirelessSerialPort(this);

    //串口数据更新
    connect(speedCurTask_1, SIGNAL(sendToSpeedGraph(speedUpdateFormat)),
            this, SLOT(serial1DataRefresh(speedUpdateFormat)),Qt::AutoConnection);
    connect(speedCurTask_2, SIGNAL(sendToSpeedGraph(speedUpdateFormat)),
            this, SLOT(serial2DataRefresh(speedUpdateFormat)),Qt::AutoConnection);
#endif
    //新建刷新用Timer(暂时)
    dataTimer = new QTimer(this);
    dataTimer->setTimerType(Qt::PreciseTimer);
    dataTimer->setInterval(20); //50帧

    //设置速度曲线显示控件并启动刷新
    plotParaSetup();
    connect(dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()),Qt::AutoConnection);  //定时刷新回调函数连接

    //本地定时器运行线程
    timeMonitor *localTotalTimeThread = new timeMonitor();  //计时线程
    localTotalTimeThread->start(); //开启计时线程

    //本地计时器对象
    QTimer *localTimer = new QTimer();
    localTimer->setInterval(1);                     //1ms间隔
    localTimer->setTimerType(Qt::PreciseTimer);     //精确定时器
    localTimer->moveToThread(localTotalTimeThread); //移动到其他线程

    //操作子线程中的定时器
    connect(this, SIGNAL(timerCTRSend(unsigned char)),
            localTotalTimeThread, SLOT(timerStatusOperation(unsigned char)), Qt::AutoConnection);   //定时开始或结束
    connect(localTotalTimeThread, SIGNAL(timerStartSig()),localTimer, SLOT(start()),Qt::AutoConnection);        //启动定时器
    connect(localTotalTimeThread, SIGNAL(timerCloseSig()),localTimer, SLOT(stop()),Qt::AutoConnection);         //停止定时器

    //打印当前计时时间戳
    connect(localTotalTimeThread, SIGNAL(cur_TimestampPrint(unsigned int)),
            this, SLOT(onTimeout(unsigned int)), Qt::QueuedConnection);   //传输当前时间戳
    //定时发送时间戳
    connect(localTimer, SIGNAL(timeout()), localTotalTimeThread, SLOT(handleEvent()),Qt::AutoConnection);        //超时执行

    //结束计时线程
    connect(this, &MainWindow::destroyed, localTotalTimeThread, [=]()
    {
        //停止计时器
        emit timerCTRSend(0);
        //等待定时器停止
        Delay_xms(10);
        //发信号停止计时线程
        if (localTotalTimeThread->isRunning())
        {
            localTotalTimeThread->quit();
            localTotalTimeThread->wait();
            //销毁计时线程
            localTotalTimeThread->deleteLater();
        }
    });

    //初始化界面
    this->initStyle();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initStyle()
{
    //加载样式表
    QFile file("C:/Users/WhiteKite2020/Desktop/styledemo/styledemo/other/qss/psblack.css");
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }
}

// 周期时钟同步 1s调用
void MainWindow::sendTimeSyncSig()
{
    uint32_t curTimeStamp;
    CANFrame_STD canpack;

    //获取最新时间
    currentTime =QTime::currentTime();

    //填写报文内容
    canpack.CANID.STDCANID.MasterOrSlave = 0; //Master下发
    canpack.CANID.STDCANID.CTRCode = 0x04;      //时间同步报文分发
    canpack.CANID.STDCANID.NodeGroupID = 0x01;  //广播

    canpack.CANData[0] = (g_FrameNum_CH[0] & 0xFF00) >> 8;
    canpack.CANData[1] = g_FrameNum_CH[0] & 0x00FF;

    canpack.CANData[2] = (globalSynTime_ms >> 16) & 0xFF;
    canpack.CANData[3] = (globalSynTime_ms >> 8) & 0xFF;
    canpack.CANData[4] = globalSynTime_ms & 0xFF;

    packetSend(0x1F, 4, (unsigned char *)(&canpack));
}

// 周期获取位置传感器反馈值 > 25ms
void MainWindow::sendRequestSig()
{
    uint32_t curTimeStamp;
    CANFrame_STD canpack;

    //获取最新时间
    currentTime =QTime::currentTime();

    //填写报文内容
    canpack.CANID.STDCANID.MasterOrSlave = 0;       //Master下发
    canpack.CANID.STDCANID.CTRCode = 0x05;          //时间同步报文分发
    canpack.CANID.STDCANID.NodeGroupID = 0x01;      //广播

    canpack.CANData[0] = 0;
    canpack.CANData[1] = 0;

    canpack.CANData[2] = (globalSynTime_ms >> 16) & 0xFF;
    canpack.CANData[3] = (globalSynTime_ms >> 8) & 0xFF;
    canpack.CANData[4] = globalSynTime_ms & 0xFF;

    packetSend(0x1F, 5, (unsigned char *)(&canpack));
}

//非预测速度给定
void MainWindow::sendGivenSpeedSig(unsigned char sendNo, short givenSpeed)       //发送单板速度给定
{
    uint32_t curTimeStamp = globalSynTime_ms;
    CANFrame_STD canpack;

    //填写报文内容
    canpack.CANID.STDCANID.MasterOrSlave = 0;   //Master下发
    canpack.CANID.STDCANID.CTRCode = 0x01;      //速度给定报文分发
    canpack.CANID.STDCANID.NodeGroupID = sendNo;  //CAN ID=1

    canpack.CANData[0] = (g_FrameNum_CH[0] & 0xFF00) >> 8;    //只有速度给定和速度反馈查询需要帧号，并且由于速度反馈是主从式的，所以帧号只能由PC端发起
    canpack.CANData[1] = g_FrameNum_CH[0] & 0x00FF;

    //指向 data[2]
    memcpy(&(canpack.CANData[2]), &curTimeStamp, 3);

    //给定速度 rpm
    canpack.CANData[5] = (givenSpeed >> 8) & 0xFF;
    canpack.CANData[6] = givenSpeed & 0xFF;

    packetSend(sendNo, 1, (unsigned char *)(&canpack));
    g_FrameNum_CH[0]++;
}

//预测速度给定(未启用)
void MainWindow::sendGivenPreSpeedSig(unsigned char sendNo, feedbackData sampleData, uint8_t prelen)    //发送单板预测给定
{
    uint32_t curTimeStamp;
    CANFrame_STD canpack[10];
    uint8_t ii =0;
    EthUARRPRE u_arr;

    //离散预测控制器
    //preControllerUpdate(sampleData, 10);

    //输出反馈控制器
//    u_arr = preControllerUpdateY();

//    //填写报文内容
//    for (ii=0;ii<prelen;ii++) {
//        canpack[ii].CANID.STDCANID.CTRCode = 0x02;      //预测速度报文分发
//        canpack[ii].CANID.STDCANID.NodeGroupID = sendNo;
//        canpack[ii].CANData[0] = (g_FrameNum_CH[0] & 0xFF00) >> 8;        //只有速度给定和速度反馈查询需要帧号，并且由于速度反馈是主从式的，所以帧号只能由PC端发起
//        canpack[ii].CANData[1] = g_FrameNum_CH[0] & 0x00FF;
//        ptr = &(canpack[ii].CANData[2]);
//        memcpy(ptr, &curTimeStamp,sizeof(curTimeStamp));
//        ptr = &(canpack[ii].CANData[5]);
//        memcpy(ptr, &(u_arr.preGivenSpeed[ii]), sizeof(short));
//    }

//    packetSend(sendNo, 2, (unsigned char *)canpack);
//    g_FrameNum_CH[0]++;
}

//超时处理
void MainWindow::onTimeout(unsigned int RecvCurTimeStamp_Ms)
{
    static unsigned int requestPacketCnt = 0;
    static unsigned int timeSyncPacketCnt = 0;
    static unsigned int rttDelaySyncCnt = 0;
    globalSynTime_ms = RecvCurTimeStamp_Ms;   //更新系统时间
    float givenSpeed = 0;
    static unsigned short pidCnt = 0;


    //刷新系统计时 500ms
    refreshCnt++;
    if(refreshCnt >= 500)
    {
        ui->label_4->setText(QString::number(globalSynTime_ms, 10));
        refreshCnt=0;
    }

    // 时间同步报文下发 1s
    if(timeSyncPacketCnt >= 1000) {
        timeSyncPacketCnt = 0;
        sendTimeSyncSig();
    }

    // 周期查询报文下发时间 40ms
    if (requestPacketCnt >= 40) {
        requestPacketCnt = 0;
        sendRequestSig();
    }

    timeSyncPacketCnt++;
    requestPacketCnt++;

//    // 位置模式
//    if (workMode == 1) {
//        if (pidCnt >= 40) {
//            givenSpeed = PIDController_Update(&pid, (float)posiRef, (float)(laFData_CH1.pulseCnt));
//            sendGivenSpeedSig(1, (short)givenSpeed);
//            pidCnt =0;
//        }
//        pidCnt++;
//    }
}

//计时开始/终止信号
void MainWindow::on_timerSet_clicked()
{
    volatile unsigned char timerStatus_Now = 0;
    if(ui->timerSet->text() == "计时开始")
    {
        timerStatus_Now =1;
        ui->timerSet->setText("计时停止");
        emit timerCTRSend(timerStatus_Now);
    }
    else if(ui->timerSet->text() == "计时停止")
    {
        timerStatus_Now =0;
        ui->timerSet->setText("计时开始");
        emit timerCTRSend(timerStatus_Now);
    }
}

//子循环延时等待
uint16_t Delay_xms(uint16_t value)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(value, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
    return 0;
}

//画刷设置
void MainWindow::plotParaSetup()
{
    QStringList lineNames; //图例曲线名称表
    QPen pen; //画笔

    //画刷及曲线颜色设定
    //1、以太网
    ui->speedRecord->addGraph(); // blue line
    ui->speedRecord->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    ui->speedRecord->addGraph(); // red line
    ui->speedRecord->graph(1)->setPen(QPen(QColor(255, 110, 40)));
    ui->speedRecord->addGraph(); // black line
    ui->speedRecord->graph(2)->setPen(QPen(QColor(0, 0, 0)));
//    //2、以太网
//    ui->speedRecord->addGraph(); // Green line
//    ui->speedRecord->graph(2)->setPen(QPen(QColor(34, 139, 34)));
//    ui->speedRecord->addGraph(); // red line
//    ui->speedRecord->graph(3)->setPen(QPen(QColor(0, 0, 0)));

    //定义坐标轴范围
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%m:%s:%z");                    //时间格式
    ui->speedRecord->xAxis->setTicker(timeTicker);            //x轴跟随本地时间
    ui->speedRecord->axisRect()->setupFullAxesBox();
    ui->speedRecord->yAxis->setRange(-100, 2000);            //y轴给定上下限

    //定义坐标轴名称
    ui->speedRecord->plotLayout()->insertRow(0);
    m_title = new QCPTextElement(ui->speedRecord,"反馈位置曲线图");
    ui->speedRecord->plotLayout()->addElement(0,0,m_title);
    ui->speedRecord->yAxis->setLabel("Posi-unit");
    ui->speedRecord->xAxis->setLabel("Time-ms");

    //允许鼠标滚轮缩放和显示范围拖拽
    ui->speedRecord->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    ui->speedRecord->axisRect()->setRangeDrag(Qt::Vertical | Qt::Horizontal);
    ui->speedRecord->setInteraction(QCP::iRangeDrag, true);
    ui->speedRecord->setInteraction(QCP::iRangeZoom, true);
    ui->speedRecord->setInteraction(QCP::iSelectPlottables, true);

    //增加图例
    ui->speedRecord->legend->setVisible(true);
    ui->speedRecord->legend->setFont(QFont("Helvetica",9)); //图例 字体
    lineNames << "PMSM1"<< "PMSM2" << "PMSM3" ;

    ui->speedRecord->graph(0)->setName(lineNames[0]);
    ui->speedRecord->graph(1)->setName(lineNames[1]);
    ui->speedRecord->graph(2)->setName(lineNames[2]);
   // ui->speedRecord->graph(0)->setLineStyle((QCPGraph::LineStyle)0);

    //make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->speedRecord->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->speedRecord->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->speedRecord->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->speedRecord->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot: 定时器循环调用realtimeDataSlot
    connect(dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()),Qt::AutoConnection);  //这里可以也用本地精密时钟，但需要考虑定时与刷新的同步规则
    dataTimer->start(0); // Interval 0 means to refresh as fast as possible
}

//实时更新槽函数
void MainWindow::realtimeDataSlot()
{
    QString dispText;
    static double lastPointKey = 0;

    //获取当前时间
    static QTime time(QTime::currentTime());
    // calculate two new data points:
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds

    //最大帧数 50帧
    if (key-lastPointKey > 0.02) // at most add point every 20 ms
    {
        //反馈位置曲线
        ui->speedRecord->graph(0)->addData(key, (int)(laFData_CH1.pulseCnt/PULSENUM));

        // rescale value (vertical) axis to fit the current data:
        ui->speedRecord->graph(0)->rescaleValueAxis(true);
        lastPointKey = key;
    }
    // make key axis range scroll with the data (at a constant range size of 8): 坐标轴根据数据长度以8的倍数缩放
    ui->speedRecord->xAxis->setRange(key, 8, Qt::AlignRight);
    //重绘曲线画面
    ui->speedRecord->replot();

    // calculate frames per second: 逐秒计算帧数
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;

    //计算平均帧率
    if (key-lastFpsKey > 2) // average fps over 2 seconds
    {
        dispText =  QString("%1 FPS, Total Data points: %2").arg(frameCount/(key-lastFpsKey), 0, 'f', 0).arg(ui->speedRecord->graph(0)->data()->size());
        statusLabel->setText(dispText);
        lastFpsKey = key;
        frameCount = 0;
    }
}

//同步停车(接口保留，指令废弃)
void MainWindow::on_synStop_clicked()
{
    volatile unsigned int cursendTime = 0;
    unsigned char data1[8] = {0};
    unsigned char data2[8] = {0};

    cursendTime = globalSynTime_ms;
    for(int i =0;i<4;i++)
    {
        data1[i] = (cursendTime >> ((3-i)*8)) | 0x000000FF;
    }
    packetSend(0x01, 0x04, data1);

    cursendTime = globalSynTime_ms;
    for(int i=0;i<4;i++)
    {
        data2[i] = (cursendTime >> ((3-i)*8)) | 0x000000FF;
    }
    packetSend(0x02, 0x04, data2);
}

//根据以太网报文更新显示
void MainWindow::pageMsgRefresh()
{
    //返回状态解包
}

//扫描当前可用串口
void MainWindow::on_pushButton_4_clicked()
{
    QStringList curAvailablePortList;
    //获取当前所有可用端口
    curAvailablePortList = speedCurTask_1->getAvailablePorts();
    //合并同名记录
    curAvailablePortList = curAvailablePortList.toSet().toList();

    //清空旧记录
    ui->comboBox->clear();
    ui->comboBox_2->clear();

    for(int i=0;i<curAvailablePortList.size();i++)
    {
        ui->comboBox->addItem(curAvailablePortList.at(i));
        ui->comboBox_2->addItem(curAvailablePortList.at(i));
    }
}

//串口1连接指定串口并加锁
void MainWindow::on_pushButton_2_clicked()
{
    bool connectResult = FALSE;
    //unsigned char currentIndexNum = 0;
    QString connectPortName = ui->comboBox->currentText();
    int connectedPortIndex = 0;

    //判断是否暂无连接
    if(ui->pushButton_2->text() == "执行器1连接")
    {
        connectResult = speedCurTask_1->openPort(connectPortName, 500000); //500kbps连接
        //为避免重复，连接成功后在串口2的候选列表中设定对应串口号不可用，并且更改连接状态
        if(connectResult == TRUE)
        {
            ui->pushButton_2->setText("执行器1断开");
            connectedPortIndex = ui->comboBox_2->findText(connectPortName, Qt::MatchFixedString);
            ui->comboBox_2->removeItem(connectedPortIndex);
        }
        else
        {
             QMessageBox::critical(0, "警告！", "串口1连接失败!",QMessageBox::Cancel);
        }
    }
    else if(ui->pushButton_2->text() == "执行器1断开")
    {
         speedCurTask_1->closePort();
         Delay_xms(10);
         ui->pushButton_2->setText("执行器1连接");
    }
    else
    {
        QMessageBox::critical(0, "警告！", "串口1状态未知！",QMessageBox::Cancel);
    }
}

//串口2连接状态变更
void MainWindow::on_pushButton_3_clicked()
{
    bool connectResult = FALSE;
    //unsigned char currentIndexNum = 0;
    QString connectPortName = ui->comboBox_2->currentText();
    int connectedPortIndex = 0;

    //判断是否暂无连接
    if(ui->pushButton_3->text() == "执行器2连接")
    {
        connectResult = speedCurTask_2->openPort(connectPortName, 500000); //500kbps连接
        //为避免重复，连接成功后在串口1的候选列表中设定对应串口号不可用，并且更改连接状态
        if(connectResult == TRUE)
        {
            ui->pushButton_3->setText("执行器2断开");
            //搜索对应串口Index
            connectedPortIndex = ui->comboBox->findText(connectPortName, Qt::MatchFixedString);
            ui->comboBox->removeItem(connectedPortIndex);
        }
        else
        {
             QMessageBox::critical(0, "警告！", "串口2连接失败!",QMessageBox::Cancel);
        }
    }
    else if(ui->pushButton_3->text() == "执行器2断开")
    {
         speedCurTask_2->closePort();
         ui->pushButton_3->setText("执行器2连接");
    }
    else
    {
         QMessageBox::critical(0, "警告！", "串口2状态未知！",QMessageBox::Cancel);
    }
}

//串口数据更新
void MainWindow::serial1DataRefresh(speedUpdateFormat updatePack)
{

}

void MainWindow::serial2DataRefresh(speedUpdateFormat updatePack)
{

}

//控制通信状态设置
void MainWindow::on_pushButton_5_clicked()
{
    if(ui->pushButton_5->text() == "启动控制通讯")
    {
        requestPacketFlag = 1;
        ui->pushButton_5->setText("停止控制通讯");
    }
    else if(ui->pushButton_5->text() == "停止控制通讯")
    {
        requestPacketFlag = 0;
         ui->pushButton_5->setText("开始控制通讯");
    }
    else
    {
        QMessageBox::critical(0, "警告！", "UDP通信状态异常！",QMessageBox::Cancel);
    }
}

/*报文类型有四种：
 * Etype =0: 无预测直接发
 * Etype =1: NPC预测控制
 * Etype =2: 时钟同步帧
 * Etpye =3: 周期获取位置反馈
 * Etype =4; 周期获取平均同步位置(单向定时上传，不下发)
*/

//发送报文:单条CAN包填充
qint64 MainWindow::packetSend(unsigned char sendNo, unsigned char NodeCmd, unsigned char *data)   //以太网报文发送
{
    EthControlFrame sendSTHCtlPack; //普通指令，控制帧
    EthPredFrame sendETHPack; //预测指令，控制帧
    qint64 sendResult = 0;
    uint8_t sendFlag =0;
    uint8_t SendBuffer[500] = {0};
    uint8_t cnt =0;

    memset(&sendSTHCtlPack, 0, sizeof(sendSTHCtlPack));
    memset(&sendETHPack, 0, sizeof(sendETHPack));

    switch(NodeCmd)
    {
        //速度给定(无预测)
        case 1:
            sendSTHCtlPack.EType = 0x01;
            if (sendNo >= 1 &&sendNo <= 3) {
                memcpy(&(sendSTHCtlPack.canCmd[sendNo-1]), data, sizeof(CANFrame_STD));
                sendSTHCtlPack.canEnable[sendNo-1] = 1;
            } else if (sendNo == 0x1F) {
                for (cnt=0; cnt<3; cnt++) {
                    data[0] += 0x40;
                    memcpy(&(sendSTHCtlPack.canCmd[cnt]), data, sizeof(CANFrame_STD));
                    sendSTHCtlPack.canEnable[cnt] = 1;
                }
            } else {
                qDebug() << "Error CANPack Num\n";
            }
            // 更新发送时间
            sendSTHCtlPack.sendTimeStamp = globalSynTime_ms;
            sendFlag = 1;
        break;

        //速度给定预测
        case 2:
            sendETHPack.EType = 0x02;
            sendETHPack.can_preLen[0] =10;
            sendETHPack.can_preLen[1] =10;
            sendETHPack.ETotalPackNum = 20;
            if (sendNo >= 1 && sendNo <= 3) {
                sendETHPack.ENum = g_FrameNum_CH[sendNo-1];
                sendSTHCtlPack.canEnable[sendNo-1] = 1;
                memcpy(sendETHPack.canPreCmd[sendNo-1], data, sizeof(CANFrame_STD)*10);
            } else if (sendNo == 0x1F) {
                for (cnt = 0; cnt <3; cnt++) {
                    memcpy(&(sendSTHCtlPack.canCmd[cnt]), data, sizeof(CANFrame_STD));
                    sendSTHCtlPack.canEnable[cnt] = 1;
                }
            } else {
                 qDebug() << "Error CANPack Num\n";
            }
            sendFlag = 2;
        break;

        case 4: //时间同步分发
            sendSTHCtlPack.EHeader = 0xAA55;
            sendSTHCtlPack.ENum = g_FrameNum_CH[0]; //暂时用CH1的帧号
            sendSTHCtlPack.EType = 0x04;

            for (cnt=0;cnt<3; cnt++) {
                data[0] += 0x40;
                sendSTHCtlPack.canEnable[cnt] = 1;
                memcpy(&(sendSTHCtlPack.canCmd[cnt]), data, sizeof(CANFrame_STD));
            }
            sendFlag = 1;

            timeStamp[g_FrameNum_CH[0]] = globalSynTime_ms;
            g_FrameNum_CH[0]++;
        break;

        case 5: //传感器位置查询
            sendSTHCtlPack.EHeader = 0xAA55;
            sendSTHCtlPack.ENum = 0; //暂时用CH1的帧号
            sendSTHCtlPack.EType = 0x05;

            for (cnt = 0;cnt<3; cnt++) {
                data[0] += 0x40;
                sendSTHCtlPack.canEnable[cnt] = 1;
                memcpy(&(sendSTHCtlPack.canCmd[cnt]), data, sizeof(CANFrame_STD));
            }
            sendFlag =1;
        break;

        default:
            qDebug() << "Error CANPack EType\n";
        break;
    }

    // 只有速度给定和位置反馈查询需要帧号，并且由于位置反馈是主从式的，所以帧号只能由PC端发起
    if (sendFlag == 1) {
        sendSTHCtlPack.EHeader = 0xAA55;
        sendSTHCtlPack.ENum = 0; //暂时为0
        sendSTHCtlPack.ELen = sizeof(sendSTHCtlPack);
        sendSTHCtlPack.sendTimeStamp = globalSynTime_ms;

        memset(SendBuffer, 0, sizeof(SendBuffer));
        memcpy(SendBuffer, &sendSTHCtlPack,sizeof(sendSTHCtlPack));
        sendResult = sendSocket->writeDatagram((char *)SendBuffer, sizeof(sendSTHCtlPack), QHostAddress(dst_IP), dst_Port);
    } else if (sendFlag == 2) {
        sendETHPack.EHeader = 0xAA55;
        sendETHPack.sendTimeStamp = globalSynTime_ms;

        memset(SendBuffer, 0, sizeof(SendBuffer));
        memcpy(SendBuffer, &sendETHPack,sizeof(sendETHPack));
        sendResult = sendSocket->writeDatagram((char *)SendBuffer, sizeof(sendETHPack), QHostAddress(dst_IP), dst_Port);
    } else {
        qDebug() << "Not Send\n";
        sendResult = 1;
    }
    return sendResult;
}


//预测通道事件触发判断
uint8_t dpetc_U_update(unsigned char sendNo, feedbackData sampleData)
{
    uint8_t updateFlag =1; //先默认都触发


    return updateFlag;
}

//环路时延计算
void MainWindow::rttDelayUpdate(unsigned char sendNo, feedbackData sampleData)
{
    static short rttDelay = 0;
    uint32_t recvStamp;
    unsigned int tmpglobalTimeMs = 0;
    uint32_t curTimeStamp;

    recvtTimeStamp[sampleData.frameNum] = globalSynTime_ms;
    rttDelay = recvtTimeStamp[sampleData.frameNum]-timeStamp[sampleData.frameNum];

    ui->label_9->setText(QString::number(rttDelay, 10));
}

//接收事件触发判断SLot函数
void MainWindow::upreupdateJudge(unsigned char sendNo, feedbackData sampleData)
{
    switch(sendNo) {
        case 1:
            laFData_CH1.sampleTimeStamp = sampleData.sampleTimeStamp;
            laFData_CH1.pulseCnt = sampleData.pulseCnt;

            if(sampleData.pulseCnt == 0) {

            } else {
                if(recPosiCnt_CH1 >= maxStorageLen-1000) {
                    recPosiCnt_CH1 = 0;
                    QMessageBox::critical(0, "警告！", "接收数据1即将存储满，请及时存储到本地！",QMessageBox::Cancel);
                }

                g_RecvTimeMS_CH1[recPosiCnt_CH1] = globalSynTime_ms;
                fDataPosi_CH1[recPosiCnt_CH1].pulseCnt = sampleData.pulseCnt;
                ui->label_12->setText(QString::number(sampleData.pulseCnt, 10));
            }
            recPosiCnt_CH1++;

        break;

        case 2:
            laFData_CH2.sampleTimeStamp = sampleData.sampleTimeStamp;
            laFData_CH2.pulseCnt = sampleData.pulseCnt;

            if(sampleData.pulseCnt == 0) {

            } else {
                if(recPosiCnt_CH2 >= maxStorageLen-1000) {
                    recPosiCnt_CH2 = 0;
                    QMessageBox::critical(0, "警告！", "接收数据2即将存储满，请及时存储到本地！",QMessageBox::Cancel);
                }

                g_RecvTimeMS_CH2[recPosiCnt_CH2] = globalSynTime_ms;
                fDataPosi_CH2[recPosiCnt_CH2].pulseCnt = sampleData.pulseCnt;
                ui->label_13->setText(QString::number(sampleData.pulseCnt, 10));
            }
            recPosiCnt_CH2++;

        case 3:
            laFData_CH3.sampleTimeStamp = sampleData.sampleTimeStamp;
            laFData_CH3.pulseCnt = sampleData.pulseCnt;

            if(sampleData.pulseCnt == 0) {

            } else {
                if(recPosiCnt_CH3 >= maxStorageLen-1000) {
                    recPosiCnt_CH3 = 0;
                    QMessageBox::critical(0, "警告！", "接收数据3即将存储满，请及时存储到本地！",QMessageBox::Cancel);
                }

                g_RecvTimeMS_CH3[recPosiCnt_CH3] = globalSynTime_ms;
                fDataPosi_CH3[recPosiCnt_CH3].pulseCnt = sampleData.pulseCnt;
                ui->label_13->setText(QString::number(sampleData.pulseCnt, 10));
            }
            recPosiCnt_CH3++;

        break;
        default:
        break;
    }

    /*
    uint8_t updateFlag =0;
    updateFlag = dpetc_U_update(sendNo, sampleData); //事件触发判断

    currentTime =QTime::currentTime();
    curTimeStamp.minute = currentTime.minute();
    curTimeStamp.second = currentTime.second();
    curTimeStamp.msec = currentTime.msec();

    if (updateFlag ==1) {
        sendGivenPreSpeedSig(sendNo, sampleData, 10);
    } else {
        qDebug("%d-%d-%d: No Need Update U_ARR\n", curTimeStamp.minute, curTimeStamp.second, curTimeStamp.msec);
    }

    */
}


void MainWindow::updateAvgPosi(short avgPosi)
{
    static unsigned short cnt = 0;

    if (cnt >= maxStorageLen) {
        cnt =0;
        qDebug() << "out of record Range!\n";
    }
    recordArray[cnt].recvTimeStamp = globalSynTime_ms;
    recordArray[cnt].avgRecord = avgPosi;
    recordArray[cnt].referSig = ui->refPosiSig->text().toInt();
    //ui->avgPosi->setText(QString::number(avgPosi,10));
}

//滑窗平均速度计算 4个周期
short AverageSpeedCal(uint32_t recvPulse)
{
    static uint32_t pulseWindow[4] = {0};
    double averageSpeed = 0;
    static uint8_t cnt = 0;
    float coeff_arr[3] = {0.25,0.3,0.45}; // 平均速度权值

    //滑窗权重 .3
    for(cnt=1;cnt<=3;cnt++)
    {
        pulseWindow[cnt-1] = pulseWindow[cnt];
    }
    pulseWindow[3]=recvPulse;

    for (cnt=1;cnt<=3;cnt++){
        averageSpeed += ((pulseWindow[cnt]- pulseWindow[cnt-1])*coeff_arr[cnt-1])*50;
    }

    return (short)(averageSpeed/(60*PULSENUM)); //rpm
}

//CAN报文更新速度值
void MainWindow::UpdateFeedbackSpeed1rpm(speedUpdateFormat curSpeedTime)
{

}

void MainWindow::UpdateFeedbackSpeed2rpm(speedUpdateFormat curSpeedTime)
{

}

//同步启动
void MainWindow::on_synStart_clicked()
{
    volatile unsigned int cursendTime = 0;
    unsigned char data1[8] = {0};
    unsigned char data2[8] = {0};

    cursendTime = globalSynTime_ms;
    for(int i =0;i<4;i++)
    {
        data1[i] = (cursendTime >> ((3-i)*8)) | 0x000000FF;
    }
    packetSend(0x01, 0x03, data1);
    qDebug() << "1发送成功";
    Delay_xms(20);

    cursendTime = globalSynTime_ms;
    for(int i =0;i<4;i++)
    {
        data2[i] = (cursendTime >> ((3-i)*8)) | 0x000000FF;
    }
    packetSend(0x02, 0x03, data2);
    qDebug() << "2发送成功";
}

//测试用：根据面板上的速度给定进行同步运动
void MainWindow::on_speedGiven_clicked()
{
    uint32_t curTimeStamp;
    CANFrame_STD canpack;

    //获取最新时间
    currentTime =QTime::currentTime();

    //填写报文内容
    canpack.CANID.STDCANID.MasterOrSlave = 0; //Master下发
    canpack.CANID.STDCANID.CTRCode = 0x01;
    canpack.CANID.STDCANID.Reserve = 0;

//    canpack.CANData[0] = (g_FrameNum_CH1 & 0xFF00) >> 8;
//    canpack.CANData[1] = g_FrameNum_CH1 & 0x00FF;
    canpack.CANData[0] = 0;
    canpack.CANData[1] = 0;

    memcpy(&(canpack.CANData[2]), &curTimeStamp,sizeof(curTimeStamp));
    canpack.CANData[5] = (ui->lineEdit->text().toInt() >> 8) & 0xFF;
    canpack.CANData[6] = (ui->lineEdit->text().toInt()) & 0xFF;

    canpack.CANID.STDCANID.NodeGroupID = 0x00;
    packetSend(0x1F, 1, (unsigned char *)(&canpack));
}

//0x1E 开启时钟同步报文分发
void MainWindow::on_pushButton_10_clicked()
{
    if(ui->pushButton_10->text() == "时钟同步启动")
    {
        timeSynFlag = 1;
        ui->pushButton_10->setText("时钟同步停止");
    }
    else if(ui->pushButton_10->text() == "时钟同步停止")
    {
        timeSynFlag = 0;
         ui->pushButton_10->setText("时钟同步启动");
    }
    else
    {
        QMessageBox::critical(0, "警告！", "时钟同步状态异常！",QMessageBox::Cancel);
        timeSynFlag = 0;
    }
}

// 离散控制器一步预测
SYSPREARR oneStepPre(double *x_esti_arr)
{
    static SYSPREARR oneStepPre;
    double x_plus1_pre[3]={0.0};
    double Matrix_A[3][3]= {{1.12,0.213,-0.335},{1,0,0},{0,1,0}};
    double Matrix_B[3] ={1,0,0};
    double Matrix_C[3] ={0.0541,0.1150,0.0001};
    double Matrix_K[3] ={-0.1200, -0.5030, -0.0050};

    //更新 x_k+1
    //pre_kplus1[0] =
    //更新 u_k+1
    oneStepPre.u_pre= Matrix_K[0]*x_esti_arr[0]+Matrix_K[1]*x_esti_arr[1]+Matrix_K[2]*x_esti_arr[2];

    return oneStepPre;
}


//输出反馈预测
EthUARRPRE MainWindow::preControllerUpdateY()
{
    EthUARRPRE u_arr;



    return u_arr;
}

// 预测控制器更新 40ms为周期
EthUARRPRE MainWindow::preControllerUpdate(uint32_t curTimeStamp, double *x_esti_arr, unsigned char prelen)
{
    //discrete Model
    uint8_t period_MS = 40;

    EthUARRPRE u_arr;
    uint8_t ii=0;
    double *temp_x_pre = x_esti_arr;
    SYSPREARR tempPrePlus;

    for (ii=0;ii<prelen;ii++) {
        tempPrePlus = oneStepPre(temp_x_pre);
        temp_x_pre = tempPrePlus.x_pre; //更新 x_k+1

    }
    return u_arr;
}

// 饱和函数近似
int satFunc(int inputS) {
    int satOut = 0;
    if (inputS > 1) {
       satOut = 1;
    } else if (inputS < -1) {
        satOut = -1;
    } else {
        satOut = inputS;
    }
    return satOut;
}

// 滑模控制器 s(k)=(I+alphaT)e
unsigned short SMCpreController(short *x)
{
    unsigned short smcOut = 0;
    short refPosi = 200; //参考位置信号（常值）

    // 跟踪控制系统参数
    return smcOut;
}


//0x1F 发送大量数据读取请求
void MainWindow::on_pushButton_6_clicked()
{
    unsigned char data1[8] = {0};
    unsigned int curSendTimeStamp = globalSynTime_ms;

    //获取时间戳
    for(int ii=0;ii<4;ii++)
    {
        data1[3-ii] = (uint8_t) (curSendTimeStamp & 0xFF);
        curSendTimeStamp >>= 8; //右移8位
    }
    data1[6] = 0x01;  //告知请求

    //向0x01发送读取请求
    packetSend(0x01, 0x1F, data1);
    //向0x02发送读取请求
    //packetSend(0x02, 0x1F, data1);
}

//存储文件路径选取
char* MainWindow::GetCuurentFilePath(void)
{
    QString aFileName;
    QString filter;
    QString dlgTitle;
    QString curPath;
    size_t FilePathLen;
    while(1)
    {
        curPath=QDir::currentPath();// get current system path
        dlgTitle="打开一个文件";
        filter="所有文件(*.*)"; //文件过滤器
        aFileName=QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);
        if(!aFileName.isEmpty())
        {
            FilePathLen = aFileName.size();
            char* FilePath = (char*)malloc(sizeof(char) * (FilePathLen+1));//allocate memory
            QByteArray ba = aFileName.toLatin1();
            strcpy(FilePath, ba.data());
            return FilePath;
        }
        else
        {
            QString dlgTitle="Warning";
            QString strInfo="You haven't chose a file, do you want to exit？";

            QMessageBox::StandardButton  defaultBtn=QMessageBox::NoButton; //缺省按钮

            QMessageBox::StandardButton result;//返回选择的按钮
            result=QMessageBox::question(this, dlgTitle, strInfo,
                              QMessageBox::Yes|QMessageBox::No,
                              defaultBtn);

            if(result==QMessageBox::No)
            {
                continue;
            }
            else
            {
                return NULL;
            }
        }
    }
}

// 参考速度信号曲线生成
short* posiRefCal(unsigned int dstPosiIncre) {
    static short *speedRef = NULL;

    return speedRef;
}

//位置环给定位移运动：pulse
void MainWindow::on_PosiLoopInit_clicked()
{
    //1、获取初始位置:以最近一次获取到的脉冲数为基本值
    ui->label_12->setText(QString::number(laFData_CH1.pulseCnt, 10));
    ui->label_13->setText(QString::number(laFData_CH2.pulseCnt, 10));
    ui->label_13->setText(QString::number(laFData_CH3.pulseCnt, 10));

    //终态参考位移给定
    posiRef = ui->refPosiSig->text().toInt();

    //初始化当前
    PIDController_Init(&pid);

    //变更系统控制模式
    workMode = 1;

    qDebug() << "Now Start the Posi Ref! posiRef is" << QString::number(posiRef, 10);
}

// 导出PMSM1记录数据
void MainWindow::on_StorePMSM1_clicked()
{
    //获取创建的csv文件名
    QString fileName = QFileDialog::getSaveFileName(this, tr("Excel file"), "" ,tr("Files (*.csv)"));
    if (fileName.isEmpty())
        return;

    //打开.csv文件
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug("Cannot open file for writing: %s",qPrintable(file.errorString()));
        return;
    }
    QTextStream out(&file);

    //创建表头
    out << tr("全局时间戳,") << tr("PMSM1反馈位置,") << ",\n";

    //写入内容
    for(int i = 0; i < recPosiCnt_CH1; i++)//写入10行
    {
        out << g_RecvTimeMS_CH1[i] << "," << fDataPosi_CH1[i].pulseCnt << "\n";
    }

    //关闭文件
    file.close();
}

// 导出PMSM2数据
void MainWindow::on_StorePMSM2_clicked()
{
    //获取创建的csv文件名
    QString fileName = QFileDialog::getSaveFileName(this, tr("Excel file"), "" ,tr("Files (*.csv)"));
    if (fileName.isEmpty())
        return;

    //打开.csv文件
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug("Cannot open file for writing: %s",qPrintable(file.errorString()));
        return;
    }
    QTextStream out(&file);

    //创建表头
    out << tr("全局时间戳,") << tr("PMSM2反馈位置,") << ",\n";

    //写入内容
    for(int i = 0; i < recPosiCnt_CH2; i++)//写入10行
    {
        out << g_RecvTimeMS_CH2[i] << "," << fDataPosi_CH2[i].pulseCnt << "\n";
    }

    //关闭文件
    file.close();
}

void MainWindow::on_StorePMSM3_clicked()
{
    //获取创建的csv文件名
    QString fileName = QFileDialog::getSaveFileName(this, tr("Excel file"), "" ,tr("Files (*.csv)"));
    if (fileName.isEmpty())
        return;

    //打开.csv文件
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug("Cannot open file for writing: %s",qPrintable(file.errorString()));
        return;
    }
    QTextStream out(&file);

    //创建表头
    out << tr("全局时间戳,") << tr("PMSM3反馈位置,") << ",\n";

    //写入内容
    for(int i = 0; i < recPosiCnt_CH3; i++)//写入10行
    {
        out << g_RecvTimeMS_CH3[i] << "," << fDataPosi_CH3[i].pulseCnt << "\n";
    }

    //关闭文件
    file.close();
}


/* 控制算法 */
