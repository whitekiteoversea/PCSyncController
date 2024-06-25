#include "mainwindow.h"
#include <QApplication>
#include <QThreadPool>
#include "FrameSheet.h"
#include <QMetaType>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon("C:/Users/WhiteKite2020/Desktop/AutoTransMonitor/other/main.ico"));
    a.setFont(QFont("Microsoft Yahei", 9));
    //a.setWindowIcon(QIcon(":/main.ico"));
    //注册传输数据结构体
    qRegisterMetaType<speedUpdateFormat>("speedUpdateFormat");
    qRegisterMetaType<feedbackData>("feedbackData");
    qRegisterMetaType<CASREPORTFRAME>("CASREPORTFRAME");

    MainWindow w;
    w.setWindowTitle("龙门负载控制子系统上位机");

    w.setWindowFlags(Qt::Widget | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    w.show();
    return a.exec();
}


