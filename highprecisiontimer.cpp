#include "highprecisiontimer.h"

HighPrecisionTimer :: HighPrecisionTimer(QObject *parent)
    : QObject{parent}
{
    runningStatus = false;
}


HighPrecisionTimer :: ~HighPrecisionTimer()
{


}

void HighPrecisionTimer :: start() { // 默认1000s
    runningStatus = true;
    timer.start();
    QTimer::singleShot(0, this, &HighPrecisionTimer::onTimeoutCheck);
}

void HighPrecisionTimer :: stop() {
    runningStatus = false;
}

void HighPrecisionTimer :: onTimeoutCheck() {
    if (runningStatus == true) {
        if (timer.elapsed() >= 1) {
            emit timeout();
            timer.restart(); // 重置定时器计时
        }
        QTimer::singleShot(0, this, &HighPrecisionTimer::onTimeoutCheck); // Schedule next check immediately
    }
}
