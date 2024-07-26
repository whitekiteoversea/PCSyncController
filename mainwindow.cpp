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

#include "pid.h"
#include "cccAlgo.h"
#include "posiAlgo.h"
#include "smcAlgo.h"
#include <cmath>


// 对于ETH发送的处理其实比较粗陋，应该初始化一个队列对象，队列对象提供一个发送接口，每次发送都是向队列对象
// 发送一次信号，这样可以有效避免发送撞车，但时间来不及了，后面测出问题再考虑增加吧

// 数据存储相关
#define maxStorageLen 12000

SUBPACK onceRecvArray[RESNUM];
unsigned int recordCnt = 0; // 单次总记录号
unsigned char transNum = 0;

// 帧号相关
#define initFrameNum_TimeSync 0
#define maxFrameNum_TimeSync 19999
#define initFrameNum_SpeedGiven 20000
#define maxFrameNum_SpeedGiven 29999
#define initFrameNum_PosiFeed 30000
#define maxFrameNum_PosiFeed 39999
#define initFrameNum_SpeedPre 40000
#define maxFrameNum_SpeedPre 59999

// 同步控制相关

volatile unsigned char PMSMCurWorkMode[2] = {0};
volatile unsigned char posiSyncModeEnabled = 0;

//可用串口列表
QStringList *availbleSerialPort;

// 单机通信测试 0  or ETH-CAS 通信 1
volatile unsigned char curAlgoMode = 0;

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
uint16_t recPosiCnt_CH[3] = {0};
feedbackData laFData_CH[3];  // 绘图使用电机反馈数据

uint32_t g_RecvTimeMS_CH1[maxStorageLen];   //存储反馈位置数据时间
uint32_t g_RecvTimeMS_CH2[maxStorageLen];   //存储反馈位置数据时间
uint32_t g_RecvTimeMS_CH3[maxStorageLen];   //存储反馈位置数据时间
feedbackData fDataPosi_CH1[maxStorageLen];       //存储反馈位置数据
feedbackData fDataPosi_CH2[maxStorageLen];       //存储反馈位置数据
feedbackData fDataPosi_CH3[maxStorageLen];       //存储反馈位置数据

// 时间同步报文帧号 0-19999
volatile unsigned short g_FrameNum_CH[3] = {initFrameNum_TimeSync,initFrameNum_TimeSync,initFrameNum_TimeSync};
// 速度给定报文帧号 20000-29999
volatile unsigned short g_speedGivenNum_CH[3] = {initFrameNum_SpeedGiven,initFrameNum_SpeedGiven,initFrameNum_SpeedGiven};
// 位置反馈报文帧号 暂时 29999-39999
volatile unsigned short g_PosiFeedNum_CH = initFrameNum_PosiFeed;
//速度预测报文帧号 40000-59999
volatile unsigned short g_PreFrameNum_CH[3] = {initFrameNum_SpeedPre, initFrameNum_SpeedPre, initFrameNum_SpeedPre};

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

// 对端IP、Port
QString dst_CASIP[2] = {"192.168.20.11","192.168.20.12"};
int dst_CASPort[2] = {8001, 8002};

QString dst_IP = "192.168.1.30";
int dst_Port = 8001;

// 全局计时 1ms
volatile unsigned int globalSynTime_ms = 0;
volatile int requestPacketFlag = 0;   //发送标志

// 位置环任务数据
int initZOffAxisPosi[2] ={0, 0}; // 起始位置
unsigned int targetPosium[2] = {0, 0};  // 目标相对位置
volatile unsigned char taskAccomplishWindowPopFlag = 0;

// 同步任务基本参数记录
POSISYNCTASK posiTask;

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
    //传输时延计算
    connect(localEthRecvTask, SIGNAL(updateRTTDelay(unsigned char, feedbackData)),
            this, SLOT(rttDelayUpdate(unsigned char, feedbackData)), Qt::AutoConnection);
    //电机状态数据
    connect(localEthRecvTask, SIGNAL(updateStatus(unsigned char, CASREPORTFRAME)),
            this, SLOT(updateRealTimeStatus(unsigned char, CASREPORTFRAME)), Qt::AutoConnection);
    //SDRAM存储数据
    connect(localEthRecvTask, SIGNAL(updateSDRAMData(unsigned char, unsigned int, unsigned int, unsigned int, SUBPACK*)),
            this, SLOT(updateSDRAMDataSlot(unsigned char, unsigned int, unsigned int, unsigned int, SUBPACK*)), Qt::AutoConnection);
    //CAS回复准备好获取
    connect(localEthRecvTask, SIGNAL(readyToSDRAMTrans(unsigned char)), this, SLOT(readyToSDRAMTransSlot(unsigned char)), Qt::AutoConnection);

#ifdef SERIAL_DEBUG_ENABLE
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

    // 设置鼠标跟踪数据浮标
    ui->speedRecord->setInteractions(QCP::iRangeDrag | QCP::iSelectLegend | QCP::iRangeZoom  | QCP::iSelectPlottables | QCP::iSelectAxes);
    connect(ui->speedRecord, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(CustomPlotMousePress(QMouseEvent*)));
    connect(ui->speedRecord, SIGNAL(selectionChangedByUser()), this, SLOT(CustomPlotSelectionChanged()));
    this->p_DataTracer = new DataTracer(ui->speedRecord);

    //本地定时器运行线程
    timeMonitor *localTotalTimeThread = new timeMonitor();  //计时线程
    localTotalTimeThread->start(); //开启计时线程

    //本地计时器对象
    QTimer *localTimer = new QTimer();
    localTimer->setInterval(1);                     //1ms间隔
    localTimer->setTimerType(Qt::PreciseTimer);     //精确定时器(其实一点也不精确)
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

void MainWindow::CustomPlotMousePress(QMouseEvent* event)
{
    // 鼠标右击查看全览
    if (event->button() == Qt::RightButton) {
        // 获取所有数据点的范围
        // ui->speedRecord->xAxis->axisRect()
        ui->speedRecord->graph(0)->rescaleAxes();
        // 重新绘制图表
        ui->speedRecord->replot();
    } else if (event->button() == Qt::LeftButton) {
        this->m_PressedPoint = event->pos();// 鼠标左击查看标签
    }
}

void MainWindow::CustomPlotSelectionChanged(){
    if (ui->speedRecord->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->speedRecord->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) || ui->speedRecord->xAxis->selectedParts().testFlag(QCPAxis::spAxisLabel))
    {
        ui->speedRecord->xAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels | QCPAxis::spAxisLabel);
        ui->speedRecord->xAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels | QCPAxis::spAxisLabel);
    }
    if (ui->speedRecord->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->speedRecord->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) || ui->speedRecord->yAxis->selectedParts().testFlag(QCPAxis::spAxisLabel))
    {
        ui->speedRecord->yAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels | QCPAxis::spAxisLabel);
        ui->speedRecord->yAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels | QCPAxis::spAxisLabel);
    }

    int graph_index = -1;
    bool haveselected=false;
    for (int i=0; i<ui->speedRecord->graphCount(); ++i)
    {
        QCPGraph *graph = ui->speedRecord->graph(i);
        graph->setVisible(false);
        QCPPlottableLegendItem *item = ui->speedRecord->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected())
        {
            //仅显示当前被选中的曲线
            graph_index = i;
            haveselected=true;
            graph->setVisible(true);
            item->setSelected(true);
            graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
        }
    }

    if(!haveselected){
        this->p_DataTracer->setVisible(false);
        for (int i=0; i<ui->speedRecord->graphCount(); ++i){
            ui->speedRecord->graph(i)->setVisible(true);
        }
    }else{
        this->p_DataTracer->setVisible(true);
        double key, value;
        FindSelectedPoint(ui->speedRecord->graph(graph_index), this->m_PressedPoint, key, value);
        QDateTime time = QCPAxisTickerDateTime::keyToDateTime(key);
        this->p_DataTracer->setText(time.toString("Time:hh:mm.ss"), QString("Depth:%1m").arg(value, 0,'f',2));
        this->p_DataTracer->updatePosition(ui->speedRecord->graph(graph_index), key, value);
    }
}

//查找距离鼠标点击位置最近的曲线上采样点的位置
void MainWindow::FindSelectedPoint(QCPGraph *graph, QPoint select_point, double &key, double &value)
{
    double temp_key, temp_value;
    graph->pixelsToCoords(select_point, temp_key, temp_value);
    QSharedPointer<QCPGraphDataContainer> tmpContainer;
    tmpContainer = graph->data();
    int low=0, high=tmpContainer->size();

    if(tmpContainer->size()<3){
        if(tmpContainer->size() == 1){
            key = tmpContainer->constBegin()->mainKey();
            value = tmpContainer->constBegin()->mainValue();
            return;
        }else if(tmpContainer->size() == 2){
            double diff1 = qAbs(tmpContainer->at(1)->mainKey()-temp_key);
            double diff2 = qAbs(tmpContainer->at(0)->mainKey()-temp_key);
            if(diff1 <= diff2){
                key =  tmpContainer->at(1)->mainKey();
                value = tmpContainer->at(1)->mainValue();
            }else{
                key = tmpContainer->at(0)->mainKey();
                value = tmpContainer->at(0)->mainValue();
            }
            return;
        }
    }

    while (high>low) {
        int mid = (low+high)>>1;
        if(temp_key == (tmpContainer->constBegin()+mid)->mainKey()){
            key = temp_key;
            value = (tmpContainer->constBegin()+mid)->mainValue();
            break;
        }else if(temp_key > (tmpContainer->constBegin()+mid)->mainKey()){
            low = mid;
        }else if(temp_key < (tmpContainer->constBegin()+mid)->mainKey()){
            high = mid;
        }
        if(high - low <= 1){
            double diff1 = qAbs((tmpContainer->constBegin()+high)->mainKey()-temp_key);
            double diff2 = qAbs((tmpContainer->constBegin()+low)->mainKey()-temp_key);
            if(diff1 <= diff2){
                key =  (tmpContainer->constBegin()+high)->mainKey();
                value = (tmpContainer->constBegin()+high)->mainValue();
            }else{
                key = (tmpContainer->constBegin()+low)->mainKey();
                value = (tmpContainer->constBegin()+low)->mainValue();
            }
            break;
        }
    }
}



void MainWindow::initStyle()
{
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

    if (curAlgoMode == 0) {
        //填写报文内容
        canpack.CANID.STDCANID.MasterOrSlave = 1;               // Master下发
        canpack.CANID.STDCANID.CTRCode = CANTimeSyncCmd;
        canpack.CANID.STDCANID.Reserve = 0;

        if (ui->checkBox->isChecked()) {
            canpack.CANID.STDCANID.NodeGroupID = 0x01;  // 左电机
            packetSend(0x01, CANTimeSyncCmd, (unsigned char *)(&canpack));
        }

        if (ui->checkBox_2->isChecked()){
            canpack.CANID.STDCANID.NodeGroupID = 0x02;  // 右电机
            packetSend(0x02, CANTimeSyncCmd, (unsigned char *)(&canpack));
        }
    } else if (curAlgoMode == 1) {
        //填写报文内容
        canpack.CANID.STDCANID.MasterOrSlave = 1;       // Master下发
        canpack.CANID.STDCANID.CTRCode = CANTimeSyncCmd;      // 时间同步报文分发
        canpack.CANID.STDCANID.NodeGroupID = 0x01;      // 广播

        canpack.CANData[0] = (g_FrameNum_CH[0] & 0xFF00) >> 8;
        canpack.CANData[1] = g_FrameNum_CH[0] & 0x00FF;

        canpack.CANData[2] = (globalSynTime_ms >> 16) & 0xFF;
        canpack.CANData[3] = (globalSynTime_ms >> 8) & 0xFF;
        canpack.CANData[4] = globalSynTime_ms & 0xFF;

        packetSend(0x1F, 4, (unsigned char *)(&canpack));
    }
}

// 周期获取位置传感器反馈值 5ms (CAS实际0.8ms左右)
void MainWindow::sendRequestSig()
{
    CANFrame_STD canpack;
    int ii = 0;

    //获取最新时间
    currentTime =QTime::currentTime();
    for (ii =0; ii<4; ii++) {
        canpack.CANData[ii] = (refreshCnt >> 8*(3-ii)) & 0xFF;
    }

    if (curAlgoMode == 0) {
        //填写报文内容
        canpack.CANID.STDCANID.MasterOrSlave = 1;               // Master下发
        canpack.CANID.STDCANID.CTRCode = CANPosiAcquireCmd;
        canpack.CANID.STDCANID.Reserve = 0;

        if (ui->checkBox->isChecked()) {
            canpack.CANID.STDCANID.NodeGroupID = 0x01;  // 左电机
            packetSend(0x01, CANPosiAcquireCmd, (unsigned char *)(&canpack)); //速度给定
        }

        if (ui->checkBox_2->isChecked()){
            canpack.CANID.STDCANID.NodeGroupID = 0x02;  // 右电机
            packetSend(0x02, CANPosiAcquireCmd, (unsigned char *)(&canpack));
        }

        canpack.CANID.STDCANID.NodeGroupID = 0x03;  // 负载X轴电机
        packetSend(0x03, CANPosiAcquireCmd, (unsigned char *)(&canpack));

    } else if (curAlgoMode == 1) {
        //填写报文内容
        canpack.CANID.STDCANID.MasterOrSlave = 1;       // Master下发
        canpack.CANID.STDCANID.CTRCode = CANPosiAcquireCmd;          //
        canpack.CANID.STDCANID.NodeGroupID = 0x01;      // 广播

        canpack.CANData[0] = (g_PosiFeedNum_CH & 0xFF00) >> 8;
        canpack.CANData[1] = g_PosiFeedNum_CH & 0x00FF;

        canpack.CANData[2] = (globalSynTime_ms >> 16) & 0xFF;
        canpack.CANData[3] = (globalSynTime_ms >> 8) & 0xFF;
        canpack.CANData[4] = globalSynTime_ms & 0xFF;

        packetSend(0x1F, 5, (unsigned char *)(&canpack));

        g_PosiFeedNum_CH++;
        if (g_PosiFeedNum_CH > maxFrameNum_PosiFeed) {
            g_PosiFeedNum_CH = initFrameNum_PosiFeed;
        }
    }
}

//非预测速度给定(暂未使用)
void MainWindow::sendGivenSpeedSig(unsigned char sendNo, short givenSpeed)       //发送单板速度给定
{
    uint32_t curTimeStamp = globalSynTime_ms;
    CANFrame_STD canpack;

    //填写报文内容
    canpack.CANID.STDCANID.MasterOrSlave = 0;   //Master下发
    canpack.CANID.STDCANID.CTRCode = 0x01;      //速度给定报文分发
    canpack.CANID.STDCANID.NodeGroupID = sendNo;  //CAN ID=1

    //指向 data[2]
    memcpy(&(canpack.CANData[2]), &curTimeStamp, 3);

    //给定速度 rpm
    canpack.CANData[5] = (givenSpeed >> 8) & 0xFF;
    canpack.CANData[6] = givenSpeed & 0xFF;

    packetSend(sendNo, 1, (unsigned char *)(&canpack));
}

//预测速度给定(未启用)
void MainWindow::sendGivenPreSpeedSig(unsigned char sendNo, feedbackData sampleData, uint8_t prelen)    //发送单板预测给定
{
    uint32_t curTimeStamp;
    CANFrame_STD canpack[10];
    uint8_t ii =0;
    EthUARRPRE u_arr;
}


void MainWindow::singleMotorPosiLoop(void)
{
    // Motor1
    if (posiSyncModeEnabled == 2) {
        singleMotorPosiTask(1, targetPosium[0], PMSMCurWorkMode[0]);
        speedGivenUpdate(1, (short)(posiPIDA.out));
        if (checkTaskAccomplish(targetPosium[0], getRelevantPositionA()) == 1) {
            speedGivenUpdate(1, 0);
            posiSyncModeEnabled = 0;
        }
    }

    // Motor2
    if (posiSyncModeEnabled == 3) {
        singleMotorPosiTask(2, targetPosium[1], PMSMCurWorkMode[1]);
        speedGivenUpdate(2, (short)(posiPIDB.out));
        if (checkTaskAccomplish(targetPosium[1], getRelevantPositionB()) == 1) {
            speedGivenUpdate(2, 0);
            posiSyncModeEnabled = 0;
        }
    }
}

//定时器时基分配函数
void MainWindow::onTimeout(unsigned int RecvCurTimeStamp_Ms)
{
    static unsigned int requestPacketCnt = 0;
    static unsigned int timeSyncPacketCnt = 0;
    static unsigned int rttDelaySyncCnt = 0;
    globalSynTime_ms = RecvCurTimeStamp_Ms;   //更新系统时间
    float givenSpeed = 0;

    //刷新系统计时 500ms
    refreshCnt++;
    if (refreshCnt >= 500) {
        ui->label_4->setText(QString::number(globalSynTime_ms, 10));

        // 检查通信模式
        if (ui->ETHMode->isChecked()) {
            curAlgoMode = 1; // ETH-CAS 预测控制模式
        }
        refreshCnt = 0;

        // 检查电机控制模式
        PMSMCurWorkMode[0] = ui->modeSet1->currentText().toInt();
        PMSMCurWorkMode[1] = ui->modeSet2->currentText().toInt();
    }

    // 周期查询报文下发时间 5ms (CAS模式下与时间同步报文合并)
    if (requestPacketCnt >= CONTROL_SENSOR_PERIOD_MS) {
        requestPacketCnt = 0;
        sendRequestSig(); //位置反馈获取

        // 位置控制输出更新
        if (posiSyncModeEnabled == 1) {
            // 控制更新
            posiSyncAlgoTask();
            // 发送给定控制
            speedGivenUpdate(1, (short)(ccc_Control.pidA.out));
            speedGivenUpdate(2, (short)(ccc_Control.pidB.out));
            // 过程指标数据记录
            dataCollection();

            if ((checkTaskAccomplish(targetPosium[0], getRelevantPositionA()) == 1) && \
                (checkTaskAccomplish(targetPosium[1], getRelevantPositionB()) == 1)) {
                // 抵达停机
                speedGivenUpdate(1, 0);
                speedGivenUpdate(2, 0);
                posiSyncModeEnabled = 0;
                qDebug() << " TASKTime: " + QString::number(dataCol.TaskTimeMS, 10) << "ms, "
                         << "MAX rotateAngle: " << QString::number(dataCol.rotateAngle_ABS_MAX, 'g', 6) << " \n";
            }
        }

        // 单电机位置环
        singleMotorPosiLoop();
    }

    // 计时++
    timeSyncPacketCnt++;
    requestPacketCnt++;
}

//计时开始/终止信号
void MainWindow::on_timerSet_clicked()
{
    volatile unsigned char timerStatus_Now = 0;
    if(ui->timerSet->text() == "计时开始") {
        timerStatus_Now = 1;
        ui->timerSet->setText("计时停止");
        emit timerCTRSend(timerStatus_Now);
    } else if(ui->timerSet->text() == "计时停止") {
        timerStatus_Now = 0;
        ui->timerSet->setText("计时开始");
        emit timerCTRSend(timerStatus_Now);
    }
}

//子循环阻塞式延时等待
uint16_t Delay_xms(uint16_t value)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(value, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
    return 0;
}

// 非阻塞式等待，基于QTimer
uint8_t nonblockingDelay_xms(uint32_t *lastTime, uint16_t delayMS)
{
    uint8_t ret = 0;


    return ret;
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
    ui->speedRecord->addGraph(); //GREEN
    ui->speedRecord->graph(2)->setPen(QPen(QColor(255, 0, 0)));
    ui->speedRecord->addGraph(); //YELLOW
    ui->speedRecord->graph(3)->setPen(QPen(QColor(255, 255, 0)));

#ifdef XAXIS_ENABLE
    ui->speedRecord->addGraph(); // black line
    ui->speedRecord->graph(2)->setPen(QPen(QColor(0, 0, 0)));
#endif

    //定义坐标轴范围
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%m:%s:%z");                    //时间格式
    ui->speedRecord->xAxis->setTicker(timeTicker);            //x轴跟随本地时间

    ui->speedRecord->axisRect()->setupFullAxesBox();
    ui->speedRecord->yAxis->setRange(-4000, 5100);            //y轴给定上下限

    //定义坐标轴名称
    ui->speedRecord->plotLayout()->insertRow(0);
    m_title = new QCPTextElement(ui->speedRecord,"反馈位置曲线图");
    ui->speedRecord->plotLayout()->addElement(0,0,m_title);
    ui->speedRecord->yAxis->setLabel("Posi-0.1mm/Torque-0.1N.m");
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
    lineNames << "PMSM1"<< "PMSM2" << "Torque1" << "Torque2";

#ifdef XAXIS_ENABLE
    lineNames << "PMSM3" ;
#endif
    for (int cnt =0; cnt <4; cnt++) {
        ui->speedRecord->graph(cnt)->setName(lineNames[cnt]);
    }

#ifdef XAXIS_ENABLE
    ui->speedRecord->graph(2)->setName(lineNames[2]);
#endif

    //make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->speedRecord->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->speedRecord->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->speedRecord->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->speedRecord->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot: 定时器循环调用realtimeDataSlot
    connect(dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()),Qt::AutoConnection);  //这里可以也用本地精密时钟，但需要考虑定时与刷新的同步规则
    dataTimer->start(0); // Interval 0 means to refresh as fast as possible
}

void MainWindow::realtimeDataSlot()
{
    QString dispText;
    static double lastPointKey = 0;
    static double lastFpsKey;
    static int frameCount;

    //获取当前时间
    static QTime time(QTime::currentTime());
    // calculate two new data points:
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds

    //最大帧数 50帧
    if (key-lastPointKey > 0.02) {
        if (laFData_CH[0].feedbackPosium <= LEFT_START_POSI) {
            ui->speedRecord->graph(0)->addData(key, 0);
        } else {
            ui->speedRecord->graph(0)->addData(key, (laFData_CH[0].feedbackPosium - LEFT_START_POSI)/100);
        }
        if (laFData_CH[1].feedbackPosium <= RIGHT_START_POSI) {
            ui->speedRecord->graph(1)->addData(key, 0);
        } else {
            ui->speedRecord->graph(1)->addData(key, (laFData_CH[1].feedbackPosium - RIGHT_START_POSI)/100);
        }

        ui->speedRecord->graph(2)->addData(key, (laFData_CH[0].motorRealTimeTorqueNM*MOTORTORQUE/100));
        ui->speedRecord->graph(3)->addData(key, (laFData_CH[1].motorRealTimeTorqueNM*MOTORTORQUE/100));

        // ReScale
        ui->speedRecord->graph(0)->rescaleValueAxis(true); // rescale value (vertical) axis to fit the current data:
        lastPointKey = key;
    } 
    ui->speedRecord->xAxis->setRange(key, 8, Qt::AlignRight); // 坐标轴根据数据长度以8的倍数缩放
    ui->speedRecord->replot();//重绘曲线画面

    ++frameCount;
    //窗口长度2s 计算平均帧率
    if (key-lastFpsKey > 2) {
        dispText =  QString("%1 FPS, Total Data points: %2").arg(frameCount/(key-lastFpsKey), 0, 'f', 0).arg(ui->speedRecord->graph(0)->data()->size());
        statusLabel->setText(dispText);
        lastFpsKey = key;
        frameCount = 0;
    }
}

// 快速停车
void MainWindow::on_synStop_clicked()
{
    volatile unsigned int cursendTime = 0;
    CANFrame_STD canpack;

    unsigned char data1[8] = {0};
    unsigned char data2[8] = {0};

    cursendTime = globalSynTime_ms;

    if (curAlgoMode == 0) {

        canpack.CANID.STDCANID.MasterOrSlave = 1;
        canpack.CANID.STDCANID.CTRCode = CANTargetCmd;
        canpack.CANID.STDCANID.Reserve = 0;

        canpack.CANData[4] = 0;
        canpack.CANData[5]= 0;
        if (ui->checkBox->isChecked()) {
            canpack.CANID.STDCANID.NodeGroupID = 0x01;  // 左电机
            packetSend(0x01, CANTargetCmd, (unsigned char *)(&canpack));
        }
        if (ui->checkBox_2->isChecked()) {
            canpack.CANID.STDCANID.NodeGroupID = 0x02;  // 右电机
            packetSend(0x02, CANTargetCmd, (unsigned char *)(&canpack));
        }
    } else if (curAlgoMode == 1) {
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
}

//根据以太网报文更新显示
void MainWindow::pageMsgRefresh()
{
    ;//返回状态解包
}

#ifdef SERIAL_DEBUG_ENABLE

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

    for(int i=0; i<curAvailablePortList.size(); i++) {
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
    if(ui->pushButton_2->text() == "执行器1连接") {
        connectResult = speedCurTask_1->openPort(connectPortName, 500000); //500kbps连接
        //为避免重复，连接成功后在串口2的候选列表中设定对应串口号不可用，并且更改连接状态
        if(connectResult == TRUE) {
            ui->pushButton_2->setText("执行器1断开");
            connectedPortIndex = ui->comboBox_2->findText(connectPortName, Qt::MatchFixedString);
            ui->comboBox_2->removeItem(connectedPortIndex);
        }
        else {
             QMessageBox::critical(0, "警告！", "串口1连接失败!",QMessageBox::Cancel);
        }
    }
    else if(ui->pushButton_2->text() == "执行器1断开") {
         speedCurTask_1->closePort();
         Delay_xms(10);
         ui->pushButton_2->setText("执行器1连接");
    }
    else {
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
    if(ui->pushButton_3->text() == "执行器2连接") {
        connectResult = speedCurTask_2->openPort(connectPortName, 500000); //500kbps连接
        //为避免重复，连接成功后在串口1的候选列表中设定对应串口号不可用，并且更改连接状态
        if (connectResult == TRUE) {
            ui->pushButton_3->setText("执行器2断开");
            //搜索对应串口Index
            connectedPortIndex = ui->comboBox->findText(connectPortName, Qt::MatchFixedString);
            ui->comboBox->removeItem(connectedPortIndex);
        }
        else {
             QMessageBox::critical(0, "警告！", "串口2连接失败!",QMessageBox::Cancel);
        }
    }
    else if (ui->pushButton_3->text() == "执行器2断开") {
         speedCurTask_2->closePort();
         ui->pushButton_3->setText("执行器2连接");
    }
    else {
         QMessageBox::critical(0, "警告！", "串口2状态未知！",QMessageBox::Cancel);
    }
}

//串口数据更新
void MainWindow::serial1DataRefresh(speedUpdateFormat updatePack)
{
    ;
}

void MainWindow::serial2DataRefresh(speedUpdateFormat updatePack)
{
    ;
}

#endif

//控制通信状态设置
void MainWindow::on_pushButton_5_clicked()
{
    if (ui->pushButton_5->text() == "启动控制通讯") {
        requestPacketFlag = 1;
        ui->pushButton_5->setText("停止控制通讯");
    } else if(ui->pushButton_5->text() == "停止控制通讯") {
        requestPacketFlag = 0;
         ui->pushButton_5->setText("开始控制通讯");
    } else {
        QMessageBox::critical(0, "警告！", "UDP通信状态异常！",QMessageBox::Cancel);
    }
}

/*报文类型有四种：
 * Etype =0: 无预测直接发
 * Etype =1: NPC预测控制
 * Etype =2: 时钟同步帧
 * Etpye =3: 周期获取位置反馈
 * Etype =4: 周期获取平均同步位置(单向定时上传，不下发)
 * Etype =0x1F: 存储数据上传
*/

//发送报文:单条CAN包填充
qint64 MainWindow::packetSend(unsigned char sendNo, unsigned char NodeCmd, unsigned char *data)   //以太网报文发送
{
    EthControlFrame sendSTHCtlPack; //普通指令，控制帧

    EthControlFrameSingleCAS PC2CASFrame; //直接面对CAS
    EthPredFrame sendETHPack; //预测指令，控制帧
    qint64 sendResult = 0;
    uint8_t sendFlag =0;
    uint8_t SendBuffer[500] = {0};
    uint8_t cnt =0;

    if (curAlgoMode == 0) { // ToCAS
        memset(&PC2CASFrame, 0, sizeof(PC2CASFrame));
        memcpy(&(PC2CASFrame.canpack), data, sizeof(CANFrame_STD));

        PC2CASFrame.EHeader = 0xAA55;
        PC2CASFrame.FrameTailer = 0x55AA;
        PC2CASFrame.EType = NodeCmd;
        PC2CASFrame.ELen = sizeof(PC2CASFrame);

        memset(SendBuffer, 0, sizeof(SendBuffer));
        memcpy(SendBuffer, &PC2CASFrame,sizeof(PC2CASFrame));

        if (ui->checkBox->isChecked())
            sendResult = sendSocket->writeDatagram((char *)SendBuffer, sizeof(PC2CASFrame), QHostAddress(dst_CASIP[0]), dst_CASPort[0]);
        if (ui->checkBox_2->isChecked())
            sendResult = sendSocket->writeDatagram((char *)SendBuffer, sizeof(PC2CASFrame), QHostAddress(dst_CASIP[1]), dst_CASPort[1]);

    } else if (curAlgoMode == 1) { //To ETH2CAS
        memset(&sendSTHCtlPack, 0, sizeof(sendSTHCtlPack));
        memset(&sendETHPack, 0, sizeof(sendETHPack));

        switch(NodeCmd) {
            case SPEEDCMD: //速度给定(无预测)
                sendSTHCtlPack.EType = 0x01;
                if (sendNo >= 1 &&sendNo <= MAXCASNODENUM) {
                    memcpy(&(sendSTHCtlPack.canCmd[sendNo-1]), data, sizeof(CANFrame_STD));
                    sendSTHCtlPack.canEnable[sendNo-1] = 1;
                } else if (sendNo == 0x1F) {
                    for (cnt=0; cnt<MAXCASNODENUM; cnt++) {
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
            case PRESPEED:
                sendETHPack.EType = 0x02;
                sendETHPack.can_preLen[0] =10;
                sendETHPack.can_preLen[1] =10;
                sendETHPack.ETotalPackNum = 20;
                if (sendNo >= 1 && sendNo <= MAXCASNODENUM) {
                    sendETHPack.ENum = g_FrameNum_CH[sendNo-1];
                    sendSTHCtlPack.canEnable[sendNo-1] = 1;
                    memcpy(sendETHPack.canPreCmd[sendNo-1], data, sizeof(CANFrame_STD)*10);
                } else if (sendNo == 0x1F) {
                    for (cnt = 0; cnt <MAXCASNODENUM; cnt++) {
                        memcpy(&(sendSTHCtlPack.canCmd[cnt]), data, sizeof(CANFrame_STD));
                        sendSTHCtlPack.canEnable[cnt] = 1;
                    }
                } else {
                     qDebug() << "Error CANPack Num\n";
                }
                sendFlag = 2;
            break;

            case TIMESYNC: //时间同步分发
                sendSTHCtlPack.EHeader = 0xAA55;
                sendSTHCtlPack.ENum = g_FrameNum_CH[0]; //暂时用CH1的帧号
                sendSTHCtlPack.EType = 0x04;

                for (cnt=0;cnt<MAXCASNODENUM; cnt++) {
                    data[0] += 0x40;
                    sendSTHCtlPack.canEnable[cnt] = 1;
                    memcpy(&(sendSTHCtlPack.canCmd[cnt]), data, sizeof(CANFrame_STD));
                    sendSTHCtlPack.canCmd[cnt].CANData[2] = (globalSynTime_ms & 0xFF0000) >> 16;
                    sendSTHCtlPack.canCmd[cnt].CANData[3] = (globalSynTime_ms & 0x00FF00) >> 8;
                    sendSTHCtlPack.canCmd[cnt].CANData[4] = (globalSynTime_ms & 0x0000FF);
                }
                sendFlag = 1;

                sendSTHCtlPack.sendTimeStamp = globalSynTime_ms;
                timeStamp[g_FrameNum_CH[0]] = globalSynTime_ms;
                g_FrameNum_CH[0]++;
            break;

            case 5: //传感器位置查询
                sendSTHCtlPack.EHeader = 0xAA55;
                sendSTHCtlPack.ENum = 0; //暂时用CH1的帧号
                sendSTHCtlPack.EType = 0x05;

                for (cnt = 0;cnt<MAXCASNODENUM; cnt++) {
                    data[0] += 0x40;
                    sendSTHCtlPack.canEnable[cnt] = 1;
                    memcpy(&(sendSTHCtlPack.canCmd[cnt]), data, sizeof(CANFrame_STD));
                    sendSTHCtlPack.canCmd[cnt].CANData[2] = (globalSynTime_ms & 0xFF0000) >> 16;
                    sendSTHCtlPack.canCmd[cnt].CANData[3] = (globalSynTime_ms & 0x00FF00) >> 8;
                    sendSTHCtlPack.canCmd[cnt].CANData[4] = (globalSynTime_ms & 0x0000FF);
                }
                sendFlag =1;
                sendSTHCtlPack.sendTimeStamp = globalSynTime_ms;
            break;

            case DATAUPLOAD: //传感器位置数据上报请求
                sendSTHCtlPack.EHeader = 0xAA55;
                sendSTHCtlPack.ENum = 0; //暂时用CH1的帧号
                sendSTHCtlPack.EType = DATAUPLOAD;

                for (cnt = 0;cnt<MAXCASNODENUM; cnt++) {
                        data[0] += 0x40;
                        sendSTHCtlPack.canEnable[cnt] = 1;
                        memcpy(&(sendSTHCtlPack.canCmd[cnt]), data, sizeof(CANFrame_STD));
                        sendSTHCtlPack.canCmd[cnt].CANData[2] = (globalSynTime_ms & 0xFF0000) >> 16;
                        sendSTHCtlPack.canCmd[cnt].CANData[3] = (globalSynTime_ms & 0x00FF00) >> 8;
                        sendSTHCtlPack.canCmd[cnt].CANData[4] = (globalSynTime_ms & 0x0000FF);
                }
                sendFlag =1;
                sendSTHCtlPack.sendTimeStamp = globalSynTime_ms;
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
            laFData_CH[0].sampleTimeStamp = sampleData.sampleTimeStamp;
            laFData_CH[0].feedbackPosium = sampleData.feedbackPosium;

            if(sampleData.feedbackPosium == 0) {

            } else {
                if (recPosiCnt_CH[0] >= maxStorageLen-1000) {
                    recPosiCnt_CH[0] = 0;
                    QMessageBox::critical(0, "警告！", "接收数据1即将存储满，请及时存储到本地！",QMessageBox::Cancel);
                }

                g_RecvTimeMS_CH1[recPosiCnt_CH[0]] = globalSynTime_ms;
                fDataPosi_CH1[recPosiCnt_CH[0]].feedbackPosium = sampleData.feedbackPosium;
                ui->PMSM2Posi->setText(QString::number(sampleData.feedbackPosium, 10));
            }
            recPosiCnt_CH[0]++;

        break;

        case 2:
            laFData_CH[1].sampleTimeStamp = sampleData.sampleTimeStamp;
            laFData_CH[1].feedbackPosium = sampleData.feedbackPosium;

            if(sampleData.feedbackPosium == 0) {

            } else {
                if(recPosiCnt_CH[1] >= maxStorageLen-1000) {
                    recPosiCnt_CH[1] = 0;
                    QMessageBox::critical(0, "警告！", "接收数据2即将存储满，请及时存储到本地！",QMessageBox::Cancel);
                }

                g_RecvTimeMS_CH2[recPosiCnt_CH[1]] = globalSynTime_ms;
                fDataPosi_CH2[recPosiCnt_CH[1]].feedbackPosium = sampleData.feedbackPosium;
                ui->PMSM1Posi->setText(QString::number(sampleData.feedbackPosium, 10));
            }
            recPosiCnt_CH[1]++;

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

void MainWindow::updateRealTimeStatus(unsigned char sendNo, CASREPORTFRAME statusData)
{
    static unsigned short cnt = 0;
    volatile static int syncErrorUM = 0;

    if (cnt >= maxStorageLen) {
        cnt =0;
        qDebug() << "out of record Range!\n";
    }

    if (sendNo == 1) {
        if ((statusData.motorPosiUM >= LEFT_START_POSI) && (statusData.motorPosiUM <= LEFT_END_POSI)) {
            ui->PMSM1STA->setText("0x"+QString::number(statusData.statusWord, 16));  //状态字
            ui->PMSM1Posi->setText(QString::number(statusData.motorPosiUM, 10));
            laFData_CH[sendNo-1].feedbackPosium = statusData.motorPosiUM;
            laFData_CH[sendNo-1].cas_gTimeMS = statusData.CAS_gTime_MS;
            laFData_CH[sendNo-1].sampleTimeStamp = statusData.localTimeMS;
            laFData_CH[sendNo-1].motorRealTimeTorqueNM = statusData.motorRealTimeTorqueNM;
            ui->torque1->setText(QString::number(statusData.motorRealTimeTorqueNM, 10));
            ui->PMSM1OM->setText(QString::number(statusData.curWorkMode, 10));
        }
    } else if (sendNo == 2) {
        if ((statusData.motorPosiUM >= RIGHT_START_POSI) && (statusData.motorPosiUM <= RIGHT_END_POSI)) {
            ui->PMSM2STA->setText("0x"+QString::number(statusData.statusWord, 16));  //状态字
            ui->PMSM2Posi->setText(QString::number(statusData.motorPosiUM, 10));
            laFData_CH[sendNo-1].feedbackPosium = statusData.motorPosiUM;
            laFData_CH[sendNo-1].sampleTimeStamp = statusData.localTimeMS;
            laFData_CH[sendNo-1].cas_gTimeMS = statusData.CAS_gTime_MS;
            laFData_CH[sendNo-1].motorRealTimeTorqueNM = statusData.motorRealTimeTorqueNM;
            ui->torque2->setText(QString::number(statusData.motorRealTimeTorqueNM, 10));
            ui->PMSM2OM->setText(QString::number(statusData.curWorkMode, 10));
        }
    } else if (sendNo == 3) {
        laFData_CH[sendNo-1].feedbackPosium = statusData.motorPosiUM;
        laFData_CH[sendNo-1].sampleTimeStamp = statusData.localTimeMS;
        laFData_CH[sendNo-1].cas_gTimeMS = statusData.CAS_gTime_MS;
    }else {
        qDebug() << "Out of Motor ID Range! \n";
    }

    // 更新同步误差
    if (ui->checkBox->isChecked() && ui->checkBox_2->isChecked()) {
        syncErrorUM = laFData_CH[0].feedbackPosium - LEFT_START_POSI - laFData_CH[1].feedbackPosium + RIGHT_START_POSI + SETUP_HIGH_COMPENSATION_US;
        ui->posiSyncError->setText(QString::number(syncErrorUM, 'f', 6));
        ui->rotateAngle->setText(QString::number( (atan((double)syncErrorUM/ZAXIS_DISTANCE)),'f',6));
    }
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

    return (short)(averageSpeed/60); //rpm
}

//CAN报文更新速度值
void MainWindow::UpdateFeedbackSpeed1rpm(speedUpdateFormat curSpeedTime)
{

}

void MainWindow::UpdateFeedbackSpeed2rpm(speedUpdateFormat curSpeedTime)
{

}

//速度模式下同步启动(暂未使用)
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
    Delay_xms(20);

    cursendTime = globalSynTime_ms;
    for(int i =0;i<4;i++)
    {
        data2[i] = (cursendTime >> ((3-i)*8)) | 0x000000FF;
    }
    packetSend(0x02, 0x03, data2);
}

//测试用：根据面板上的速度给定进行运动
void MainWindow::on_speedGiven_clicked()
{
    uint32_t curTimeStamp;
    CANFrame_STD canpack;
    uint8_t cnt =0;

    if (curAlgoMode == 0){

        //填写报文内容
        canpack.CANID.STDCANID.MasterOrSlave = 1; //Master下发
        canpack.CANID.STDCANID.CTRCode = 0x03;
        canpack.CANID.STDCANID.Reserve = 0;

        canpack.CANData[4] = ((ui->lineEdit->text().toInt()) >> 8) & 0xFF;
        canpack.CANData[5] = ((ui->lineEdit->text().toInt())) & 0xFF;

        if (ui->checkBox->isChecked()) {
            canpack.CANID.STDCANID.NodeGroupID = 0x01;  // 左电机
            packetSend(0x01, CANTargetCmd, (unsigned char *)(&canpack)); //指令给定
        }

        if (ui->checkBox_2->isChecked()){
            canpack.CANID.STDCANID.NodeGroupID = 0x02;  // 右电机
            packetSend(0x02, CANTargetCmd, (unsigned char *)(&canpack));
        }
    } else {
        //获取最新时间
        currentTime =QTime::currentTime();

        //填写报文内容
        canpack.CANID.STDCANID.MasterOrSlave = 0; //Master下发
        canpack.CANID.STDCANID.CTRCode = 0x01;
        canpack.CANID.STDCANID.Reserve = 0;

        canpack.CANData[0] = (g_speedGivenNum_CH[0] & 0xFF00) >> 8;    //只有速度给定和速度反馈查询需要帧号，并且由于速度反馈是主从式的，所以帧号只能由PC端发起
        canpack.CANData[1] = g_speedGivenNum_CH[0] & 0x00FF;

        memcpy(&(canpack.CANData[2]), &curTimeStamp,sizeof(curTimeStamp));
        canpack.CANData[5] = (ui->lineEdit->text().toInt() >> 8) & 0xFF;
        canpack.CANData[6] = (ui->lineEdit->text().toInt()) & 0xFF;

        canpack.CANID.STDCANID.NodeGroupID = 0x00;
        packetSend(0x1F, 1, (unsigned char *)(&canpack));

        g_speedGivenNum_CH[0]++;
        if (g_speedGivenNum_CH[0] > maxFrameNum_SpeedGiven) {
           g_speedGivenNum_CH[0] = initFrameNum_SpeedGiven;
        }
    }
}

//提供给同步算法进行发送
unsigned char MainWindow::speedGivenUpdate(unsigned char sendNo, short giveControlInput)
{
    unsigned char ret =0;
    CANFrame_STD canpack;
    if (curAlgoMode == 0) {
        canpack.CANID.STDCANID.MasterOrSlave = 1; //Master下发
        canpack.CANID.STDCANID.CTRCode = 0x03;
        canpack.CANID.STDCANID.Reserve = 0;
        canpack.CANData[4] = (giveControlInput >> 8) & 0xFF;
        canpack.CANData[5] = (giveControlInput & 0xFF);
        canpack.CANID.STDCANID.NodeGroupID = sendNo;
        packetSend(sendNo, CANTargetCmd, (unsigned char *)(&canpack));
    }
    return ret;
}

//0x1E 开启时钟同步报文分发
void MainWindow::on_pushButton_10_clicked()
{
    if(ui->pushButton_10->text() == "时钟同步启动") {
        timeSynFlag = 1;
        ui->pushButton_10->setText("时钟同步停止");
    } else if(ui->pushButton_10->text() == "时钟同步停止") {
        timeSynFlag = 0;
         ui->pushButton_10->setText("时钟同步启动");
    } else {
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

// 预测控制器更新 5ms为周期
EthUARRPRE MainWindow::preControllerUpdate(uint32_t curTimeStamp, double *x_esti_arr, unsigned char prelen)
{
    //discrete Model
    uint8_t period_MS = 5;

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

//存储文件路径选取
char* MainWindow::GetCuurentFilePath(void)
{
    QString aFileName;
    QString filter;
    QString dlgTitle;
    QString curPath;
    size_t FilePathLen;
    while(1) {
        curPath=QDir::currentPath();// get current system path
        dlgTitle="打开一个文件";
        filter="所有文件(*.*)"; //文件过滤器
        aFileName=QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);
        if(!aFileName.isEmpty()) {
            FilePathLen = aFileName.size();
            char* FilePath = (char*)malloc(sizeof(char) * (FilePathLen+1));//allocate memory
            QByteArray ba = aFileName.toLatin1();
            strcpy(FilePath, ba.data());
            return FilePath;
        } else {
            QString dlgTitle="Warning";
            QString strInfo="You haven't chose a file, do you want to exit？";

            QMessageBox::StandardButton  defaultBtn=QMessageBox::NoButton; //缺省按钮

            QMessageBox::StandardButton result;//返回选择的按钮
            result=QMessageBox::question(this, dlgTitle, strInfo,
                              QMessageBox::Yes|QMessageBox::No,
                              defaultBtn);

            if(result==QMessageBox::No) {
                continue;
            }
            else {
                return NULL;
            }
        }
    }
}

void num2constStr(unsigned int num, const char* str)
{
    QString qstr = QString::number(num);

    // Convert QString to QByteArray
    QByteArray byteArray = qstr.toUtf8(); // or qstr.toLatin1() if you prefer Latin1 encoding

    // Get char* from QByteArray
    str = byteArray.constData();
}

void MainWindow::sdramDataSave(unsigned char saveCASID)
{
    unsigned int cnt = 0;
    //1.选择导出的csv文件保存路径
    QString csvFile = QFileDialog::getExistingDirectory(this);
    if (csvFile.isEmpty())
        return;

    //2.文件名采用CASID+系统时间戳生成唯一的文件
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy_MM_dd_hh_mm_ss");
    csvFile += tr("/CAS%1_SDRAMFileSave_export_%2.csv").arg(saveCASID).arg(current_date);

    //3.用QFile打开.csv文件 如果不存在则会自动新建一个新的文件
    QFile file(csvFile);
    if (file.exists()) {
        //如果文件存在执行的操作，此处为空，因为文件不可能存在
    }
    file.open( QIODevice::ReadWrite | QIODevice::Text );
    statusBar()->showMessage(tr("正在导出数据..."));
    QTextStream out(&file);

    //4.获取数据 创建表头
    out<<tr("GlobalTimeMS,")<<tr("LocalTimeMS,")<<tr("realTimePosi_um,")<<tr("realTimeTorque_1000p,\n");//表头
    for (cnt = 0; cnt<recordCnt; cnt++) {
        out << onceRecvArray[cnt].g_time_ms << "," << onceRecvArray[cnt].l_time_ms << "," << onceRecvArray[cnt].posi_um << "\n";
    }

    //5.写完数据需要关闭文件
    file.close();
}

// 参考速度信号曲线生成
short* posiRefCal(unsigned int dstPosiIncre) {
    static short *speedRef = NULL;

    return speedRef;
}

//位置环给定位移运动：um
void MainWindow::on_PosiLoopInit_clicked()
{
    //1、获取初始位置:以最近一次获取到的脉冲数为基本值
    ui->PMSM1Posi->setText(QString::number(laFData_CH[0].feedbackPosium, 10));
    ui->PMSM2Posi->setText(QString::number(laFData_CH[1].feedbackPosium, 10));
    // ui->label_13->setText(QString::number(laFData_CH[2].feedbackPosium, 10));

    //终态参考位移给定
    posiRef = ui->refPosiSig->text().toInt();

    //初始化当前
    PIDController_Init(&pid);

    //变更系统控制模式
    workMode = 1;
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
    for(int i = 0; i < recPosiCnt_CH[0]; i++)//写入10行
    {
        out << g_RecvTimeMS_CH1[i] << "," << fDataPosi_CH1[i].feedbackPosium << "\n";
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
    for(int i = 0; i < recPosiCnt_CH[1]; i++)//写入10行
    {
        out << g_RecvTimeMS_CH2[i] << "," << fDataPosi_CH2[i].feedbackPosium << "\n";
    }

    //关闭文件
    file.close();
}

// 交叉耦合同步更新
uint8_t crossCouplingSync_Update()
{
   uint8_t duss_result = 0;






   return duss_result;
}

// 0x0A 发送大量数据读取请求 告知CAS停止获取新数据 需要CAS处于静止时生效
// data[4]为子类型 01为请求 02为传输
void MainWindow::on_readCAS_clicked()
{
   CANFrame_STD canpack;
   unsigned int curSendTimeStamp = globalSynTime_ms;

   //获取时间戳
   for(int ii=0;ii<4;ii++) {
        canpack.CANData[3-ii] = (uint8_t) (curSendTimeStamp & 0xFF);
        curSendTimeStamp >>= 8; //右移8位
   }
   canpack.CANID.STDCANID.CTRCode = 0x0A;
   canpack.CANID.STDCANID.MasterOrSlave = 1;

   if (ui->readCAS->text() == "请求CAS传输") {
        if (curAlgoMode == 0) {
            // 停止时钟发送
            on_timerSet_clicked();
            canpack.CANData[4] = 0x00;  // 告知请求
            if (ui->checkBox->isChecked() && ui->checkBox_2->isChecked()) {
                QMessageBox::critical(0, "警告！", "只能同时启动对一个CAS节点的数据读取！.",QMessageBox::Cancel);
                return;
            }

            if (ui->checkBox->isChecked()) {
                canpack.CANID.STDCANID.NodeGroupID = 1;
                packetSend(0x01, 0x0A, ((uint8_t *)&canpack));
            } else if (ui->checkBox_2->isChecked()) {
                canpack.CANID.STDCANID.NodeGroupID = 2;
                packetSend(0x02, 0x0A, ((uint8_t *)&canpack));
            }
        } else if (curAlgoMode == 1) {
            ;
        }
        ui->CASUploadStatus->setText("等待请求回复");
   } else if (ui->readCAS->text() == "开始CAS传输") {
        canpack.CANID.STDCANID.NodeGroupID = 1;
        canpack.CANData[4] = 0x01;
        packetSend(transNum, 0x0A, ((uint8_t *)&canpack));
        ui->CASUploadStatus->setText("等待CAS传输完成");
   }
}


// CAS 工作模式设置 0x03
void MainWindow::on_PMSM1workModeSetup_clicked()
{
   CANFrame_STD canpack;
   if (curAlgoMode == 0){
        canpack.CANID.STDCANID.MasterOrSlave = 1; //Master下发
        canpack.CANID.STDCANID.CTRCode = 0x00; // 在以太网中未使用, 仅ETH-CAS模式下有效
        canpack.CANID.STDCANID.Reserve = 0;

        if (ui->checkBox->isChecked()) {
            canpack.CANID.STDCANID.NodeGroupID = 0x01;  // 左电机
            canpack.CANData[4] = ui->modeSet1->currentText().toInt();
            packetSend(0x01, CANOperationModeCmd, (unsigned char *)(&canpack)); //指令给定
        }
    }
}


void MainWindow::on_PMSM2workModeSetup_clicked()
{
    CANFrame_STD canpack;
    if (curAlgoMode == 0) {
        canpack.CANID.STDCANID.MasterOrSlave = 1; //Master下发
        canpack.CANID.STDCANID.CTRCode = 0x00; // 在以太网中未使用, 仅ETH-CAS模式下有效
        canpack.CANID.STDCANID.Reserve = 0;

        if (ui->checkBox_2->isChecked()){
            canpack.CANID.STDCANID.NodeGroupID = 0x02;  // 右电机
            canpack.CANData[4] = ui->modeSet2->currentText().toInt();
            packetSend(0x02, CANOperationModeCmd, (unsigned char *)(&canpack));
        }
    }
}

// 位置环同步算法使能
void MainWindow::on_PosiLoopSyncInit_clicked()
{
    int cmp = 0;
    taskAccomplishWindowPopFlag = 0;
    unsigned char workmode = 0;

    // 记录初始位移
    initZOffAxisPosi[0] = laFData_CH[0].feedbackPosium;
    initZOffAxisPosi[1] = laFData_CH[1].feedbackPosium;

    if (ui->DualMotorPosi->isChecked()) {
        // 给定数据范围检查
        cmp = ui->refPosiSig->text().toInt() + initZOffAxisPosi[0];
        if ((cmp < LEFT_START_POSI) || (cmp > LEFT_END_POSI)) {
            QMessageBox::critical(0, "警告！", "Z轴左电机预设运动目标超限，请重新设置目标位置坐标！...",QMessageBox::Cancel);
            goto __end;
        }
        posiTask.relevantInitPosi[0] = cmp;

        cmp = ui->refPosiSig->text().toInt() + initZOffAxisPosi[1];
        if ((cmp < RIGHT_START_POSI) || (cmp > RIGHT_END_POSI)) {
            QMessageBox::critical(0, "警告！", "Z轴右电机预设运动目标超限，请重新设置目标位置坐标！...",QMessageBox::Cancel);
            posiTask.relevantInitPosi[0] = 0;
            goto __end;
        }
        posiTask.relevantInitPosi[1] = cmp;
        posiTask.taskPosiUM = ui->refPosiSig->text().toInt(); // 设定目标任务距离

        // 同步控制算法选取
        #if CCC_ALGO_ENABLE
            controllerInit();
        #else
            //smcAlgoInit();
        #endif
        posiSyncModeEnabled = 1; // 启动同步运动
    } else {
        if ((ui->checkBox->isChecked()) && (ui->checkBox_2->isChecked())) {
            QMessageBox::critical(0, "警告！", "当前为单机位置环调试模式！若需双机同步控制重新选择调试模式！...",QMessageBox::Cancel);
            goto __end;
        }
        if (ui->checkBox->isChecked()) {
            targetPosium[0] = ui->refPosiSig->text().toInt() + (initZOffAxisPosi[0] - LEFT_START_POSI);
            ui->PMSM1TargetPosi->setText(QString::number((targetPosium[0]+LEFT_START_POSI), 10));
            workmode = PMSMCurWorkMode[0];
            PIDController_Init_WorkMode(&posiPIDA, workmode);
            posiSyncModeEnabled = 2; // 启动单机z1位置环运动
        }
        if (ui->checkBox_2->isChecked()) {
            targetPosium[1] = ui->refPosiSig->text().toInt() + (initZOffAxisPosi[1] - RIGHT_START_POSI);
            ui->PMSM2TargetPosi->setText(QString::number((targetPosium[1]+RIGHT_START_POSI), 10));
            workmode = PMSMCurWorkMode[1];
            PIDController_Init_WorkMode(&posiPIDB, workmode);
            posiSyncModeEnabled = 3; // 启动单机z2位置环运动
        }
    }

__end:
    statusLabel->setText(" Sync Task Setup Failed!");
}

// 基本同步控制
void posiSyncAlgoTask(void)
{
    // 检查当前同步误差
    #if CCC_ALGO_ENABLE
        controlLoopWithWorkMode(posiTask.taskPosiUM, PMSMCurWorkMode[0]); // 同步时双电机控制模式应一致
    #else
        // 更新任务目标代价函数(旋转角尽量小)
    #endif
    // 输出两个Z轴电机的参考速度 / 参考转矩
    // 控制转矩得尽量快一些，速度可以相对低一些

    // 发包
}

// 负载同步控制

void MainWindow::on_PosiLoopInit_2_clicked()
{
    posiSyncModeEnabled = 0;

    targetPosium[0] = 0;
    targetPosium[1] = 0;

    initZOffAxisPosi[0] = 0;
    initZOffAxisPosi[1] = 0;

    ui->PMSM1TargetPosi->setText(QString::number(0, 10));
    ui->PMSM2TargetPosi->setText(QString::number(0, 10));

    on_synStop_clicked();
}

void MainWindow::updateSDRAMDataSlot(unsigned char sendNo, unsigned int currentSubPackNum, unsigned int totalPackNum, unsigned int writeNum, SUBPACK* array)
{
    static unsigned int totalNum = 0;
    unsigned int cnt = 0;

    if (sendNo == 0x01) {
        if (currentSubPackNum == totalPackNum) {
            totalNum += totalPackNum * SUBPACKNUM + writeNum; // 总写入项数量
            // 调用写入文件函数
            sdramDataSave(sendNo);
            totalNum = 0;
             QMessageBox::information(0, "警告！", "数据接收已完成！！...",QMessageBox::Cancel);
            ui->CASUploadStatus->setText("CAS请求数据接收已完成！");
             ui->readCAS->setText("请求CAS传输");
        } else if(currentSubPackNum < totalPackNum) {
            while (cnt <= writeNum) {
                onceRecvArray[recordCnt].g_time_ms = array[cnt].g_time_ms;
                onceRecvArray[recordCnt].l_time_ms = array[cnt].l_time_ms;
                onceRecvArray[recordCnt].posi_um = array[cnt].posi_um;
                recordCnt++;
                cnt++;
            }
        }
    } else if (sendNo == 0x02) {
        if (currentSubPackNum == totalPackNum) {
            totalNum += totalPackNum * SUBPACKNUM + writeNum; // 总写入项数量
            // 调用写入文件函数

            if (ui->readCAS->text() == "等待CAS传输完成") {
                ui->readCAS->setText("请求CAS传输");
            }

            totalNum = 0;
            recordCnt = 0;

        } else if(currentSubPackNum < totalPackNum) {
            while (cnt <= writeNum) {
                onceRecvArray[recordCnt].g_time_ms = array[cnt].g_time_ms;
                onceRecvArray[recordCnt].l_time_ms = array[cnt].l_time_ms;
                onceRecvArray[recordCnt].posi_um = array[cnt].posi_um;

                recordCnt++;
                cnt++;
            }
        }
    }
}

void MainWindow::readyToSDRAMTransSlot(unsigned char sendNo)
{
    if (ui->readCAS->text() == "请求CAS传输") {
        transNum = sendNo;
        ui->readCAS->setText("开始CAS传输");
    }
}
