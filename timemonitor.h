#ifndef TIMEMONITOR_H
#define TIMEMONITOR_H

#include <QObject>
#include <QThread>

class timeMonitor : public QThread
{
     Q_OBJECT
public:
    explicit timeMonitor(QObject* parent = nullptr);
    ~timeMonitor();

signals:
    void cur_TimestampPrint(unsigned int curTime_Ms);

    //定时器操作
    void timerCloseSig();  //对定时器：关闭定时器
    void timerStartSig();  //对定时器：开启定时器

public slots:
    void timingNowStart(); //计时启动：发出timerCloseSig()信号
    void timingNowStop();  //计时停止：发出timerStartSig()信号

    void timerStatusOperation(unsigned char OperationCmd); //接收控制指令的处理函数
    void handleEvent(); //定时器回调函数

private:
    int ThreadCun;
};

//volatile int totalAccuTime_Ms;  //时间累计

#endif // TIMEMONITOR_H
