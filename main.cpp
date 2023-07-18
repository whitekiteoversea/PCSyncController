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
    //qRegisterMetaType<speedUpdateFormat>("speedUpdateFormat");
    qRegisterMetaType<feedbackData>("feedbackData");

    MainWindow w;
    w.setWindowTitle("仓库转运网络化控制系统运动控制上位机");

    w.setWindowFlags(Qt::Widget | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    //w.setFixedSize(971,828);
    w.show();
    return a.exec();
}


