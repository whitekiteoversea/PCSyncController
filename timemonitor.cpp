#include "timemonitor.h"

volatile unsigned int curTime = 0; //当前计时时间

timeMonitor::timeMonitor(QObject* parent)
{

}

timeMonitor::~timeMonitor()
{

}

//SLOT 定时器开关操作
void timeMonitor::timerStatusOperation(unsigned char OperationCmd)
{
    if(OperationCmd == 0)
    {
        emit timerCloseSig(); //关闭定时器
    }
    else if(OperationCmd == 1)
    {
        //开启定时器
        emit timerStartSig();
    }
}

//SLOT 定时器回调槽函数 本地计时
void timeMonitor::handleEvent()
{
    curTime++;
    if(curTime >= 3600000) //计时1小时清零
    {
        curTime = 0;
    }
    emit cur_TimestampPrint(curTime);
}

//启动计时器
void timeMonitor::timingNowStart()
{
    emit timerStartSig();
}

//关闭计时器
void timeMonitor::timingNowStop()
{
    emit timerCloseSig();
}
