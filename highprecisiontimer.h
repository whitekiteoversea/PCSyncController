#ifndef HIGHPRECISIONTIMER_H
#define HIGHPRECISIONTIMER_H

#include <QMainWindow>
#include <QElapsedTimer>
#include <QThread>
#include <QDebug>
#include <QTime>
#include <QTimer>

class HighPrecisionTimer : public QObject {
    Q_OBJECT

public:
    HighPrecisionTimer(QObject *parent = nullptr);
    ~HighPrecisionTimer();

signals:
    void timeout(); // 触发计时线程1ms++动作

public slots:
    void start();
    void stop();

private slots:
    void onTimeoutCheck();

private:
    QElapsedTimer timer;
    bool runningStatus;
};

#endif // HIGHPRECISIONTIMER_H
