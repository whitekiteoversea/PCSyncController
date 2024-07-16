/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <qcustomplot.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGroupBox *groupBox;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox_6;
    QWidget *layoutWidget1;
    QGridLayout *gridLayout_4;
    QPushButton *pushButton_5;
    QLabel *label_3;
    QLineEdit *lineEdit;
    QPushButton *speedGiven;
    QPushButton *synStart;
    QPushButton *synStop;
    QSpacerItem *verticalSpacer;
    QCustomPlot *speedRecord;
    QGroupBox *groupBox_2;
    QWidget *layoutWidget2;
    QGridLayout *gridLayout_2;
    QSpacerItem *horizontalSpacer_2;
    QGroupBox *groupBox_5;
    QWidget *layoutWidget3;
    QVBoxLayout *verticalLayout_4;
    QPushButton *pushButton_11;
    QPushButton *pushButton_10;
    QPushButton *pushButton;
    QGroupBox *groupBox_4;
    QPushButton *timerSet;
    QLabel *label_5;
    QLabel *label_4;
    QLabel *label_6;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_4;
    QGroupBox *groupBox_3;
    QWidget *layoutWidget4;
    QGridLayout *gridLayout;
    QLabel *label_10;
    QLabel *label_2;
    QLabel *label_9;
    QComboBox *comboBox;
    QLabel *label_8;
    QLabel *label;
    QPushButton *pushButton_3;
    QComboBox *comboBox_2;
    QPushButton *pushButton_2;
    QPushButton *pushButton_4;
    QGroupBox *groupBox_8;
    QWidget *layoutWidget5;
    QGridLayout *gridLayout_6;
    QPushButton *PosiLoopInit_2;
    QLabel *PMSM2Posi;
    QPushButton *PosiLoopSyncInit;
    QLabel *torque1;
    QLabel *label_26;
    QLabel *label_24;
    QLineEdit *refPosiSig;
    QLabel *label_27;
    QLabel *torque2;
    QLabel *PMSM1Posi;
    QLabel *label_25;
    QLabel *label_28;
    QGroupBox *groupBox_9;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *ETHMode;
    QLabel *label_31;
    QComboBox *modeSet1;
    QPushButton *PMSM1workModeSetup;
    QCheckBox *checkBox;
    QLabel *label_32;
    QComboBox *modeSet2;
    QPushButton *PMSM2workModeSetup;
    QCheckBox *checkBox_2;
    QGroupBox *groupBox_10;
    QWidget *layoutWidget6;
    QGridLayout *gridLayout_3;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_33;
    QLabel *PMSM1STA;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_39;
    QLabel *posiSyncError;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_40;
    QLabel *rotateAngle;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_43;
    QLabel *PMSM1OM;
    QVBoxLayout *verticalLayout_6;
    QLabel *label_34;
    QLabel *PMSM2STA;
    QVBoxLayout *verticalLayout_7;
    QLabel *label_44;
    QLabel *PMSM2OM;
    QWidget *layoutWidget7;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox_7;
    QPushButton *readCAS;
    QLabel *CASUploadStatus;
    QLabel *label_29;
    QLabel *label_35;
    QSpacerItem *horizontalSpacer_5;
    QGroupBox *groupBox_11;
    QLabel *label_37;
    QWidget *layoutWidget8;
    QVBoxLayout *verticalLayout_8;
    QPushButton *readFile;
    QPushButton *StorePMSM1;
    QPushButton *StorePMSM2;
    QLabel *label_38;
    QLabel *label_36;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1188, 861);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMaximumSize(QSize(1920, 1080));
        MainWindow->setBaseSize(QSize(971, 828));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(23, 225, 761, 561));
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        groupBox->setMinimumSize(QSize(741, 541));
        layoutWidget = new QWidget(groupBox);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(20, 20, 702, 542));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        groupBox_6 = new QGroupBox(layoutWidget);
        groupBox_6->setObjectName("groupBox_6");
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(1);
        sizePolicy1.setHeightForWidth(groupBox_6->sizePolicy().hasHeightForWidth());
        groupBox_6->setSizePolicy(sizePolicy1);
        groupBox_6->setMinimumSize(QSize(700, 50));
        groupBox_6->setMaximumSize(QSize(1400, 200));
        layoutWidget1 = new QWidget(groupBox_6);
        layoutWidget1->setObjectName("layoutWidget1");
        layoutWidget1->setGeometry(QRect(11, 20, 681, 30));
        gridLayout_4 = new QGridLayout(layoutWidget1);
        gridLayout_4->setObjectName("gridLayout_4");
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        pushButton_5 = new QPushButton(layoutWidget1);
        pushButton_5->setObjectName("pushButton_5");

        gridLayout_4->addWidget(pushButton_5, 0, 0, 1, 1);

        label_3 = new QLabel(layoutWidget1);
        label_3->setObjectName("label_3");

        gridLayout_4->addWidget(label_3, 0, 1, 1, 1);

        lineEdit = new QLineEdit(layoutWidget1);
        lineEdit->setObjectName("lineEdit");
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(lineEdit->sizePolicy().hasHeightForWidth());
        lineEdit->setSizePolicy(sizePolicy2);
        lineEdit->setMinimumSize(QSize(113, 21));
        lineEdit->setAlignment(Qt::AlignCenter);

        gridLayout_4->addWidget(lineEdit, 0, 2, 1, 1);

        speedGiven = new QPushButton(layoutWidget1);
        speedGiven->setObjectName("speedGiven");

        gridLayout_4->addWidget(speedGiven, 0, 3, 1, 1);

        synStart = new QPushButton(layoutWidget1);
        synStart->setObjectName("synStart");
        synStart->setEnabled(false);

        gridLayout_4->addWidget(synStart, 0, 4, 1, 1);

        synStop = new QPushButton(layoutWidget1);
        synStop->setObjectName("synStop");

        gridLayout_4->addWidget(synStop, 0, 5, 1, 1);


        verticalLayout->addWidget(groupBox_6);

        verticalSpacer = new QSpacerItem(18, 18, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        speedRecord = new QCustomPlot(layoutWidget);
        speedRecord->setObjectName("speedRecord");
        speedRecord->setMinimumSize(QSize(700, 460));
        speedRecord->setMaximumSize(QSize(1400, 920));

        verticalLayout->addWidget(speedRecord);

        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(23, 12, 761, 201));
        sizePolicy.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy);
        groupBox_2->setMinimumSize(QSize(761, 151));
        layoutWidget2 = new QWidget(groupBox_2);
        layoutWidget2->setObjectName("layoutWidget2");
        layoutWidget2->setGeometry(QRect(11, 21, 727, 171));
        gridLayout_2 = new QGridLayout(layoutWidget2);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_2 = new QSpacerItem(17, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_2, 0, 0, 1, 1);

        groupBox_5 = new QGroupBox(layoutWidget2);
        groupBox_5->setObjectName("groupBox_5");
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(groupBox_5->sizePolicy().hasHeightForWidth());
        groupBox_5->setSizePolicy(sizePolicy3);
        groupBox_5->setMinimumSize(QSize(150, 121));
        layoutWidget3 = new QWidget(groupBox_5);
        layoutWidget3->setObjectName("layoutWidget3");
        layoutWidget3->setGeometry(QRect(10, 20, 131, 116));
        verticalLayout_4 = new QVBoxLayout(layoutWidget3);
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        pushButton_11 = new QPushButton(layoutWidget3);
        pushButton_11->setObjectName("pushButton_11");

        verticalLayout_4->addWidget(pushButton_11);

        pushButton_10 = new QPushButton(layoutWidget3);
        pushButton_10->setObjectName("pushButton_10");
        pushButton_10->setEnabled(false);

        verticalLayout_4->addWidget(pushButton_10);

        pushButton = new QPushButton(layoutWidget3);
        pushButton->setObjectName("pushButton");

        verticalLayout_4->addWidget(pushButton);


        gridLayout_2->addWidget(groupBox_5, 0, 1, 1, 1);

        groupBox_4 = new QGroupBox(layoutWidget2);
        groupBox_4->setObjectName("groupBox_4");
        QSizePolicy sizePolicy4(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy4);
        groupBox_4->setMinimumSize(QSize(150, 121));
        timerSet = new QPushButton(groupBox_4);
        timerSet->setObjectName("timerSet");
        timerSet->setGeometry(QRect(30, 20, 91, 23));
        label_5 = new QLabel(groupBox_4);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(10, 50, 101, 16));
        label_4 = new QLabel(groupBox_4);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(20, 80, 81, 16));
        label_6 = new QLabel(groupBox_4);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(110, 80, 31, 16));

        gridLayout_2->addWidget(groupBox_4, 0, 5, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(18, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_3, 0, 2, 1, 1);

        horizontalSpacer = new QSpacerItem(17, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 0, 6, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(17, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_4, 0, 4, 1, 1);

        groupBox_3 = new QGroupBox(layoutWidget2);
        groupBox_3->setObjectName("groupBox_3");
        sizePolicy4.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy4);
        groupBox_3->setMinimumSize(QSize(320, 121));
        layoutWidget4 = new QWidget(groupBox_3);
        layoutWidget4->setObjectName("layoutWidget4");
        layoutWidget4->setGeometry(QRect(10, 23, 311, 108));
        gridLayout = new QGridLayout(layoutWidget4);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_10 = new QLabel(layoutWidget4);
        label_10->setObjectName("label_10");

        gridLayout->addWidget(label_10, 4, 2, 1, 1);

        label_2 = new QLabel(layoutWidget4);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 3, 0, 1, 1);

        label_9 = new QLabel(layoutWidget4);
        label_9->setObjectName("label_9");

        gridLayout->addWidget(label_9, 4, 1, 1, 1);

        comboBox = new QComboBox(layoutWidget4);
        comboBox->setObjectName("comboBox");

        gridLayout->addWidget(comboBox, 2, 1, 1, 1);

        label_8 = new QLabel(layoutWidget4);
        label_8->setObjectName("label_8");

        gridLayout->addWidget(label_8, 4, 0, 1, 1);

        label = new QLabel(layoutWidget4);
        label->setObjectName("label");
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy5);

        gridLayout->addWidget(label, 2, 0, 1, 1);

        pushButton_3 = new QPushButton(layoutWidget4);
        pushButton_3->setObjectName("pushButton_3");

        gridLayout->addWidget(pushButton_3, 3, 2, 1, 1);

        comboBox_2 = new QComboBox(layoutWidget4);
        comboBox_2->setObjectName("comboBox_2");

        gridLayout->addWidget(comboBox_2, 3, 1, 1, 1);

        pushButton_2 = new QPushButton(layoutWidget4);
        pushButton_2->setObjectName("pushButton_2");

        gridLayout->addWidget(pushButton_2, 2, 2, 1, 1);

        pushButton_4 = new QPushButton(layoutWidget4);
        pushButton_4->setObjectName("pushButton_4");

        gridLayout->addWidget(pushButton_4, 1, 0, 1, 1);


        gridLayout_2->addWidget(groupBox_3, 0, 3, 1, 1);

        groupBox_8 = new QGroupBox(centralwidget);
        groupBox_8->setObjectName("groupBox_8");
        groupBox_8->setGeometry(QRect(800, 230, 361, 261));
        sizePolicy.setHeightForWidth(groupBox_8->sizePolicy().hasHeightForWidth());
        groupBox_8->setSizePolicy(sizePolicy);
        groupBox_8->setMinimumSize(QSize(120, 221));
        groupBox_8->setBaseSize(QSize(120, 221));
        layoutWidget5 = new QWidget(groupBox_8);
        layoutWidget5->setObjectName("layoutWidget5");
        layoutWidget5->setGeometry(QRect(30, 30, 301, 211));
        gridLayout_6 = new QGridLayout(layoutWidget5);
        gridLayout_6->setObjectName("gridLayout_6");
        gridLayout_6->setContentsMargins(0, 0, 0, 0);
        PosiLoopInit_2 = new QPushButton(layoutWidget5);
        PosiLoopInit_2->setObjectName("PosiLoopInit_2");

        gridLayout_6->addWidget(PosiLoopInit_2, 4, 1, 1, 1);

        PMSM2Posi = new QLabel(layoutWidget5);
        PMSM2Posi->setObjectName("PMSM2Posi");
        PMSM2Posi->setAlignment(Qt::AlignCenter);

        gridLayout_6->addWidget(PMSM2Posi, 3, 1, 1, 1);

        PosiLoopSyncInit = new QPushButton(layoutWidget5);
        PosiLoopSyncInit->setObjectName("PosiLoopSyncInit");

        gridLayout_6->addWidget(PosiLoopSyncInit, 4, 0, 1, 1);

        torque1 = new QLabel(layoutWidget5);
        torque1->setObjectName("torque1");

        gridLayout_6->addWidget(torque1, 7, 0, 1, 1);

        label_26 = new QLabel(layoutWidget5);
        label_26->setObjectName("label_26");

        gridLayout_6->addWidget(label_26, 2, 0, 1, 1);

        label_24 = new QLabel(layoutWidget5);
        label_24->setObjectName("label_24");

        gridLayout_6->addWidget(label_24, 0, 0, 1, 2);

        refPosiSig = new QLineEdit(layoutWidget5);
        refPosiSig->setObjectName("refPosiSig");

        gridLayout_6->addWidget(refPosiSig, 1, 0, 1, 2);

        label_27 = new QLabel(layoutWidget5);
        label_27->setObjectName("label_27");

        gridLayout_6->addWidget(label_27, 3, 0, 1, 1);

        torque2 = new QLabel(layoutWidget5);
        torque2->setObjectName("torque2");

        gridLayout_6->addWidget(torque2, 7, 1, 1, 1);

        PMSM1Posi = new QLabel(layoutWidget5);
        PMSM1Posi->setObjectName("PMSM1Posi");
        PMSM1Posi->setAlignment(Qt::AlignCenter);

        gridLayout_6->addWidget(PMSM1Posi, 2, 1, 1, 1);

        label_25 = new QLabel(layoutWidget5);
        label_25->setObjectName("label_25");

        gridLayout_6->addWidget(label_25, 5, 0, 1, 1);

        label_28 = new QLabel(layoutWidget5);
        label_28->setObjectName("label_28");

        gridLayout_6->addWidget(label_28, 5, 1, 1, 1);

        groupBox_9 = new QGroupBox(centralwidget);
        groupBox_9->setObjectName("groupBox_9");
        groupBox_9->setGeometry(QRect(800, 510, 151, 284));
        sizePolicy.setHeightForWidth(groupBox_9->sizePolicy().hasHeightForWidth());
        groupBox_9->setSizePolicy(sizePolicy);
        groupBox_9->setMinimumSize(QSize(120, 261));
        groupBox_9->setBaseSize(QSize(120, 261));
        verticalLayout_2 = new QVBoxLayout(groupBox_9);
        verticalLayout_2->setObjectName("verticalLayout_2");
        ETHMode = new QCheckBox(groupBox_9);
        ETHMode->setObjectName("ETHMode");

        verticalLayout_2->addWidget(ETHMode);

        label_31 = new QLabel(groupBox_9);
        label_31->setObjectName("label_31");

        verticalLayout_2->addWidget(label_31);

        modeSet1 = new QComboBox(groupBox_9);
        modeSet1->addItem(QString());
        modeSet1->addItem(QString());
        modeSet1->addItem(QString());
        modeSet1->addItem(QString());
        modeSet1->setObjectName("modeSet1");
        modeSet1->setEditable(false);

        verticalLayout_2->addWidget(modeSet1);

        PMSM1workModeSetup = new QPushButton(groupBox_9);
        PMSM1workModeSetup->setObjectName("PMSM1workModeSetup");

        verticalLayout_2->addWidget(PMSM1workModeSetup);

        checkBox = new QCheckBox(groupBox_9);
        checkBox->setObjectName("checkBox");
        checkBox->setTristate(true);

        verticalLayout_2->addWidget(checkBox);

        label_32 = new QLabel(groupBox_9);
        label_32->setObjectName("label_32");

        verticalLayout_2->addWidget(label_32);

        modeSet2 = new QComboBox(groupBox_9);
        modeSet2->addItem(QString());
        modeSet2->addItem(QString());
        modeSet2->addItem(QString());
        modeSet2->addItem(QString());
        modeSet2->setObjectName("modeSet2");

        verticalLayout_2->addWidget(modeSet2);

        PMSM2workModeSetup = new QPushButton(groupBox_9);
        PMSM2workModeSetup->setObjectName("PMSM2workModeSetup");

        verticalLayout_2->addWidget(PMSM2workModeSetup);

        checkBox_2 = new QCheckBox(groupBox_9);
        checkBox_2->setObjectName("checkBox_2");

        verticalLayout_2->addWidget(checkBox_2);

        groupBox_10 = new QGroupBox(centralwidget);
        groupBox_10->setObjectName("groupBox_10");
        groupBox_10->setGeometry(QRect(960, 510, 201, 284));
        sizePolicy2.setHeightForWidth(groupBox_10->sizePolicy().hasHeightForWidth());
        groupBox_10->setSizePolicy(sizePolicy2);
        groupBox_10->setMinimumSize(QSize(120, 261));
        groupBox_10->setBaseSize(QSize(120, 261));
        layoutWidget6 = new QWidget(groupBox_10);
        layoutWidget6->setObjectName("layoutWidget6");
        layoutWidget6->setGeometry(QRect(12, 30, 181, 231));
        gridLayout_3 = new QGridLayout(layoutWidget6);
        gridLayout_3->setObjectName("gridLayout_3");
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        label_33 = new QLabel(layoutWidget6);
        label_33->setObjectName("label_33");

        verticalLayout_3->addWidget(label_33);

        PMSM1STA = new QLabel(layoutWidget6);
        PMSM1STA->setObjectName("PMSM1STA");
        PMSM1STA->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(PMSM1STA);


        gridLayout_3->addLayout(verticalLayout_3, 0, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label_39 = new QLabel(layoutWidget6);
        label_39->setObjectName("label_39");

        horizontalLayout_2->addWidget(label_39);

        posiSyncError = new QLabel(layoutWidget6);
        posiSyncError->setObjectName("posiSyncError");

        horizontalLayout_2->addWidget(posiSyncError);


        gridLayout_3->addLayout(horizontalLayout_2, 2, 0, 1, 3);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        label_40 = new QLabel(layoutWidget6);
        label_40->setObjectName("label_40");

        horizontalLayout_3->addWidget(label_40);

        rotateAngle = new QLabel(layoutWidget6);
        rotateAngle->setObjectName("rotateAngle");

        horizontalLayout_3->addWidget(rotateAngle);


        gridLayout_3->addLayout(horizontalLayout_3, 3, 0, 1, 3);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName("verticalLayout_5");
        label_43 = new QLabel(layoutWidget6);
        label_43->setObjectName("label_43");

        verticalLayout_5->addWidget(label_43);

        PMSM1OM = new QLabel(layoutWidget6);
        PMSM1OM->setObjectName("PMSM1OM");
        PMSM1OM->setAlignment(Qt::AlignCenter);

        verticalLayout_5->addWidget(PMSM1OM);


        gridLayout_3->addLayout(verticalLayout_5, 0, 1, 1, 2);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName("verticalLayout_6");
        label_34 = new QLabel(layoutWidget6);
        label_34->setObjectName("label_34");

        verticalLayout_6->addWidget(label_34);

        PMSM2STA = new QLabel(layoutWidget6);
        PMSM2STA->setObjectName("PMSM2STA");
        PMSM2STA->setAlignment(Qt::AlignCenter);

        verticalLayout_6->addWidget(PMSM2STA);


        gridLayout_3->addLayout(verticalLayout_6, 1, 0, 1, 1);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setObjectName("verticalLayout_7");
        label_44 = new QLabel(layoutWidget6);
        label_44->setObjectName("label_44");

        verticalLayout_7->addWidget(label_44);

        PMSM2OM = new QLabel(layoutWidget6);
        PMSM2OM->setObjectName("PMSM2OM");
        PMSM2OM->setAlignment(Qt::AlignCenter);

        verticalLayout_7->addWidget(PMSM2OM);


        gridLayout_3->addLayout(verticalLayout_7, 1, 1, 1, 2);

        layoutWidget7 = new QWidget(centralwidget);
        layoutWidget7->setObjectName("layoutWidget7");
        layoutWidget7->setGeometry(QRect(800, 20, 314, 191));
        horizontalLayout = new QHBoxLayout(layoutWidget7);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        groupBox_7 = new QGroupBox(layoutWidget7);
        groupBox_7->setObjectName("groupBox_7");
        sizePolicy.setHeightForWidth(groupBox_7->sizePolicy().hasHeightForWidth());
        groupBox_7->setSizePolicy(sizePolicy);
        groupBox_7->setMinimumSize(QSize(141, 151));
        readCAS = new QPushButton(groupBox_7);
        readCAS->setObjectName("readCAS");
        readCAS->setGeometry(QRect(12, 30, 101, 24));
        sizePolicy2.setHeightForWidth(readCAS->sizePolicy().hasHeightForWidth());
        readCAS->setSizePolicy(sizePolicy2);
        CASUploadStatus = new QLabel(groupBox_7);
        CASUploadStatus->setObjectName("CASUploadStatus");
        CASUploadStatus->setGeometry(QRect(12, 142, 91, 16));
        sizePolicy3.setHeightForWidth(CASUploadStatus->sizePolicy().hasHeightForWidth());
        CASUploadStatus->setSizePolicy(sizePolicy3);
        label_29 = new QLabel(groupBox_7);
        label_29->setObjectName("label_29");
        label_29->setGeometry(QRect(12, 164, 16, 16));
        sizePolicy3.setHeightForWidth(label_29->sizePolicy().hasHeightForWidth());
        label_29->setSizePolicy(sizePolicy3);
        label_35 = new QLabel(groupBox_7);
        label_35->setObjectName("label_35");
        label_35->setGeometry(QRect(12, 120, 81, 16));
        sizePolicy3.setHeightForWidth(label_35->sizePolicy().hasHeightForWidth());
        label_35->setSizePolicy(sizePolicy3);

        horizontalLayout->addWidget(groupBox_7);

        horizontalSpacer_5 = new QSpacerItem(18, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_5);

        groupBox_11 = new QGroupBox(layoutWidget7);
        groupBox_11->setObjectName("groupBox_11");
        sizePolicy.setHeightForWidth(groupBox_11->sizePolicy().hasHeightForWidth());
        groupBox_11->setSizePolicy(sizePolicy);
        groupBox_11->setMinimumSize(QSize(141, 151));
        label_37 = new QLabel(groupBox_11);
        label_37->setObjectName("label_37");
        label_37->setGeometry(QRect(12, 164, 16, 16));
        sizePolicy3.setHeightForWidth(label_37->sizePolicy().hasHeightForWidth());
        label_37->setSizePolicy(sizePolicy3);
        layoutWidget8 = new QWidget(groupBox_11);
        layoutWidget8->setObjectName("layoutWidget8");
        layoutWidget8->setGeometry(QRect(12, 30, 111, 141));
        verticalLayout_8 = new QVBoxLayout(layoutWidget8);
        verticalLayout_8->setObjectName("verticalLayout_8");
        verticalLayout_8->setContentsMargins(0, 0, 0, 0);
        readFile = new QPushButton(layoutWidget8);
        readFile->setObjectName("readFile");
        sizePolicy2.setHeightForWidth(readFile->sizePolicy().hasHeightForWidth());
        readFile->setSizePolicy(sizePolicy2);

        verticalLayout_8->addWidget(readFile);

        StorePMSM1 = new QPushButton(layoutWidget8);
        StorePMSM1->setObjectName("StorePMSM1");
        sizePolicy2.setHeightForWidth(StorePMSM1->sizePolicy().hasHeightForWidth());
        StorePMSM1->setSizePolicy(sizePolicy2);

        verticalLayout_8->addWidget(StorePMSM1);

        StorePMSM2 = new QPushButton(layoutWidget8);
        StorePMSM2->setObjectName("StorePMSM2");
        sizePolicy2.setHeightForWidth(StorePMSM2->sizePolicy().hasHeightForWidth());
        StorePMSM2->setSizePolicy(sizePolicy2);

        verticalLayout_8->addWidget(StorePMSM2);

        label_38 = new QLabel(layoutWidget8);
        label_38->setObjectName("label_38");
        sizePolicy3.setHeightForWidth(label_38->sizePolicy().hasHeightForWidth());
        label_38->setSizePolicy(sizePolicy3);

        verticalLayout_8->addWidget(label_38);

        label_36 = new QLabel(layoutWidget8);
        label_36->setObjectName("label_36");
        sizePolicy3.setHeightForWidth(label_36->sizePolicy().hasHeightForWidth());
        label_36->setSizePolicy(sizePolicy3);

        verticalLayout_8->addWidget(label_36);


        horizontalLayout->addWidget(groupBox_11);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1188, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\272\345\217\215\351\246\210\346\225\260\346\215\256\345\217\257\350\247\206\345\214\226\346\230\276\347\244\272", nullptr));
        groupBox_6->setTitle(QCoreApplication::translate("MainWindow", "\346\216\247\345\210\266\351\200\232\344\277\241\350\256\276\347\275\256", nullptr));
        pushButton_5->setText(QCoreApplication::translate("MainWindow", "\345\220\257\345\212\250\350\275\256\350\257\242", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\347\273\231\345\256\232:rpm/0.001Nm\357\274\232", nullptr));
        lineEdit->setText(QCoreApplication::translate("MainWindow", "50", nullptr));
        speedGiven->setText(QCoreApplication::translate("MainWindow", "\346\214\207\344\273\244\347\273\231\345\256\232", nullptr));
        synStart->setText(QCoreApplication::translate("MainWindow", "\345\220\214\346\255\245\345\220\257\345\212\250", nullptr));
        synStop->setText(QCoreApplication::translate("MainWindow", "\345\220\214\346\255\245\345\201\234\350\275\246", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "\347\263\273\347\273\237\346\216\247\345\210\266\351\235\242\346\235\277", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("MainWindow", "\351\200\232\344\277\241\346\216\247\345\210\266", nullptr));
        pushButton_11->setText(QCoreApplication::translate("MainWindow", "\345\274\200\346\234\272\350\207\252\346\243\200", nullptr));
        pushButton_10->setText(QCoreApplication::translate("MainWindow", "\346\227\266\351\222\237\345\220\214\346\255\245\345\220\257\345\212\250", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\220\257\351\242\204\346\265\213\350\241\245\345\201\277", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("MainWindow", "\346\234\254\345\234\260UTC\350\256\241\346\227\266", nullptr));
        timerSet->setText(QCoreApplication::translate("MainWindow", "\350\256\241\346\227\266\345\274\200\345\247\213", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "\346\234\254\346\254\241\350\277\220\350\241\214\346\227\266\351\227\264:", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "ms", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("MainWindow", "CAS\344\270\262\345\217\243\347\233\221\346\216\247\350\277\236\346\216\245", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\2722\357\274\232", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "\345\271\263\345\235\207\346\227\266\345\273\266(ms):", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\2721\357\274\232", nullptr));
        pushButton_3->setText(QCoreApplication::translate("MainWindow", "\346\211\247\350\241\214\345\231\2502\350\277\236\346\216\245", nullptr));
        pushButton_2->setText(QCoreApplication::translate("MainWindow", "\346\211\247\350\241\214\345\231\2501\350\277\236\346\216\245", nullptr));
        pushButton_4->setText(QCoreApplication::translate("MainWindow", "\346\211\253\346\217\217\345\217\257\350\277\236\346\216\245\345\257\271\350\261\241", nullptr));
        groupBox_8->setTitle(QCoreApplication::translate("MainWindow", "\344\275\215\347\275\256\347\216\257\350\277\207\347\250\213\345\220\214\346\255\245\346\216\247\345\210\266", nullptr));
        PosiLoopInit_2->setText(QCoreApplication::translate("MainWindow", "\347\264\247\346\200\245\345\210\266\345\212\250", nullptr));
        PMSM2Posi->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        PosiLoopSyncInit->setText(QCoreApplication::translate("MainWindow", "\344\275\215\347\275\256\345\220\214\346\255\245\350\277\220\345\212\250\347\256\227\346\263\225\345\220\257\345\212\250", nullptr));
        torque1->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        label_26->setText(QCoreApplication::translate("MainWindow", "PMSM1\344\275\215\347\275\256(um)", nullptr));
        label_24->setText(QCoreApplication::translate("MainWindow", "\347\273\231\345\256\232\344\275\215\347\247\273\345\242\236\351\207\217(um)\357\274\232", nullptr));
        label_27->setText(QCoreApplication::translate("MainWindow", "PMSM2\344\275\215\347\275\256(um)", nullptr));
        torque2->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        PMSM1Posi->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        label_25->setText(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\2721\350\275\254\347\237\251(0.001Nm)", nullptr));
        label_28->setText(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\2722\350\275\254\347\237\251(0.001Nm)", nullptr));
        groupBox_9->setTitle(QCoreApplication::translate("MainWindow", "\347\263\273\347\273\237\350\277\220\350\241\214\346\250\241\345\274\217\346\216\247\345\210\266", nullptr));
        ETHMode->setText(QCoreApplication::translate("MainWindow", "ETH-CAS\346\250\241\345\274\217", nullptr));
        label_31->setText(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\2721\350\277\220\350\241\214\346\250\241\345\274\217", nullptr));
        modeSet1->setItemText(0, QCoreApplication::translate("MainWindow", "3", nullptr));
        modeSet1->setItemText(1, QCoreApplication::translate("MainWindow", "4", nullptr));
        modeSet1->setItemText(2, QCoreApplication::translate("MainWindow", "1", nullptr));
        modeSet1->setItemText(3, QCoreApplication::translate("MainWindow", "0", nullptr));

        PMSM1workModeSetup->setText(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\2721\350\256\276\347\275\256", nullptr));
        checkBox->setText(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\2721\344\275\277\350\203\275", nullptr));
        label_32->setText(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\2722\350\277\220\350\241\214\346\250\241\345\274\217", nullptr));
        modeSet2->setItemText(0, QCoreApplication::translate("MainWindow", "3", nullptr));
        modeSet2->setItemText(1, QCoreApplication::translate("MainWindow", "4", nullptr));
        modeSet2->setItemText(2, QCoreApplication::translate("MainWindow", "1", nullptr));
        modeSet2->setItemText(3, QCoreApplication::translate("MainWindow", "0", nullptr));

        PMSM2workModeSetup->setText(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\2722\350\256\276\347\275\256", nullptr));
        checkBox_2->setText(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\2722\344\275\277\350\203\275", nullptr));
        groupBox_10->setTitle(QCoreApplication::translate("MainWindow", "\347\263\273\347\273\237\350\277\220\350\241\214\347\212\266\346\200\201", nullptr));
        label_33->setText(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\2721\347\212\266\346\200\201\345\255\227", nullptr));
        PMSM1STA->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        label_39->setText(QCoreApplication::translate("MainWindow", "\345\220\214\346\255\245\350\257\257\345\267\256\357\274\210um\357\274\211\357\274\232", nullptr));
        posiSyncError->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        label_40->setText(QCoreApplication::translate("MainWindow", "\347\255\211\346\225\210\346\250\252\346\242\201\346\227\213\350\275\254\350\247\222\357\274\232\302\260", nullptr));
        rotateAngle->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        label_43->setText(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\2721\346\216\247\345\210\266\346\250\241\345\274\217", nullptr));
        PMSM1OM->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        label_34->setText(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\2722\347\212\266\346\200\201\345\255\227", nullptr));
        PMSM2STA->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        label_44->setText(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\2722\346\216\247\345\210\266\346\250\241\345\274\217", nullptr));
        PMSM2OM->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        groupBox_7->setTitle(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\350\216\267\345\217\226", nullptr));
        readCAS->setText(QCoreApplication::translate("MainWindow", "\350\257\267\346\261\202CAS\344\274\240\350\276\223", nullptr));
        CASUploadStatus->setText(QCoreApplication::translate("MainWindow", "\347\251\272\351\227\262\344\270\255", nullptr));
        label_29->setText(QString());
        label_35->setText(QCoreApplication::translate("MainWindow", "\344\274\240\350\276\223\347\212\266\346\200\201:", nullptr));
        groupBox_11->setTitle(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\350\257\273\345\217\226", nullptr));
        label_37->setText(QString());
        readFile->setText(QCoreApplication::translate("MainWindow", "\350\257\267\346\261\202\350\257\273\345\217\226", nullptr));
        StorePMSM1->setText(QCoreApplication::translate("MainWindow", "\350\257\273\345\217\226PMSM1", nullptr));
        StorePMSM2->setText(QCoreApplication::translate("MainWindow", "\350\257\273\345\217\226PMSM2", nullptr));
        label_38->setText(QCoreApplication::translate("MainWindow", "\350\257\273\345\217\226\347\212\266\346\200\201:", nullptr));
        label_36->setText(QCoreApplication::translate("MainWindow", "\346\255\243\345\234\250\350\257\273\345\217\226", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
