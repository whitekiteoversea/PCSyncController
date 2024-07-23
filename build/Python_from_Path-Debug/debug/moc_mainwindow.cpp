/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSMainWindowENDCLASS = QtMocHelpers::stringData(
    "MainWindow",
    "timerCTRSend",
    "",
    "presetTimerStatus",
    "ethernetCloseCTRSend",
    "sendRefSineSig",
    "sendCnt",
    "onTimeout",
    "recvCurTimeStamp_Ms",
    "on_timerSet_clicked",
    "pageMsgRefresh",
    "realtimeDataSlot",
    "CustomPlotMousePress",
    "QMouseEvent*",
    "event",
    "CustomPlotSelectionChanged",
    "on_synStop_clicked",
    "upreupdateJudge",
    "sendNo",
    "feedbackData",
    "sampleData",
    "rttDelayUpdate",
    "updateRealTimeStatus",
    "CASREPORTFRAME",
    "statusData",
    "updateSDRAMDataSlot",
    "currentSubPackNum",
    "totalPackNum",
    "writeNum",
    "SUBPACK*",
    "array",
    "readyToSDRAMTransSlot",
    "UpdateFeedbackSpeed1rpm",
    "speedUpdateFormat",
    "curSpeedTime",
    "UpdateFeedbackSpeed2rpm",
    "on_pushButton_5_clicked",
    "on_synStart_clicked",
    "on_speedGiven_clicked",
    "on_pushButton_10_clicked",
    "on_PosiLoopInit_clicked",
    "on_StorePMSM2_clicked",
    "on_StorePMSM1_clicked",
    "on_readCAS_clicked",
    "on_PMSM1workModeSetup_clicked",
    "on_PMSM2workModeSetup_clicked",
    "on_PosiLoopSyncInit_clicked",
    "on_PosiLoopInit_2_clicked"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {
    uint offsetsAndSizes[96];
    char stringdata0[11];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[18];
    char stringdata4[21];
    char stringdata5[15];
    char stringdata6[8];
    char stringdata7[10];
    char stringdata8[20];
    char stringdata9[20];
    char stringdata10[15];
    char stringdata11[17];
    char stringdata12[21];
    char stringdata13[13];
    char stringdata14[6];
    char stringdata15[27];
    char stringdata16[19];
    char stringdata17[16];
    char stringdata18[7];
    char stringdata19[13];
    char stringdata20[11];
    char stringdata21[15];
    char stringdata22[21];
    char stringdata23[15];
    char stringdata24[11];
    char stringdata25[20];
    char stringdata26[18];
    char stringdata27[13];
    char stringdata28[9];
    char stringdata29[9];
    char stringdata30[6];
    char stringdata31[22];
    char stringdata32[24];
    char stringdata33[18];
    char stringdata34[13];
    char stringdata35[24];
    char stringdata36[24];
    char stringdata37[20];
    char stringdata38[22];
    char stringdata39[25];
    char stringdata40[24];
    char stringdata41[22];
    char stringdata42[22];
    char stringdata43[19];
    char stringdata44[30];
    char stringdata45[30];
    char stringdata46[28];
    char stringdata47[26];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSMainWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSMainWindowENDCLASS_t qt_meta_stringdata_CLASSMainWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 10),  // "MainWindow"
        QT_MOC_LITERAL(11, 12),  // "timerCTRSend"
        QT_MOC_LITERAL(24, 0),  // ""
        QT_MOC_LITERAL(25, 17),  // "presetTimerStatus"
        QT_MOC_LITERAL(43, 20),  // "ethernetCloseCTRSend"
        QT_MOC_LITERAL(64, 14),  // "sendRefSineSig"
        QT_MOC_LITERAL(79, 7),  // "sendCnt"
        QT_MOC_LITERAL(87, 9),  // "onTimeout"
        QT_MOC_LITERAL(97, 19),  // "recvCurTimeStamp_Ms"
        QT_MOC_LITERAL(117, 19),  // "on_timerSet_clicked"
        QT_MOC_LITERAL(137, 14),  // "pageMsgRefresh"
        QT_MOC_LITERAL(152, 16),  // "realtimeDataSlot"
        QT_MOC_LITERAL(169, 20),  // "CustomPlotMousePress"
        QT_MOC_LITERAL(190, 12),  // "QMouseEvent*"
        QT_MOC_LITERAL(203, 5),  // "event"
        QT_MOC_LITERAL(209, 26),  // "CustomPlotSelectionChanged"
        QT_MOC_LITERAL(236, 18),  // "on_synStop_clicked"
        QT_MOC_LITERAL(255, 15),  // "upreupdateJudge"
        QT_MOC_LITERAL(271, 6),  // "sendNo"
        QT_MOC_LITERAL(278, 12),  // "feedbackData"
        QT_MOC_LITERAL(291, 10),  // "sampleData"
        QT_MOC_LITERAL(302, 14),  // "rttDelayUpdate"
        QT_MOC_LITERAL(317, 20),  // "updateRealTimeStatus"
        QT_MOC_LITERAL(338, 14),  // "CASREPORTFRAME"
        QT_MOC_LITERAL(353, 10),  // "statusData"
        QT_MOC_LITERAL(364, 19),  // "updateSDRAMDataSlot"
        QT_MOC_LITERAL(384, 17),  // "currentSubPackNum"
        QT_MOC_LITERAL(402, 12),  // "totalPackNum"
        QT_MOC_LITERAL(415, 8),  // "writeNum"
        QT_MOC_LITERAL(424, 8),  // "SUBPACK*"
        QT_MOC_LITERAL(433, 5),  // "array"
        QT_MOC_LITERAL(439, 21),  // "readyToSDRAMTransSlot"
        QT_MOC_LITERAL(461, 23),  // "UpdateFeedbackSpeed1rpm"
        QT_MOC_LITERAL(485, 17),  // "speedUpdateFormat"
        QT_MOC_LITERAL(503, 12),  // "curSpeedTime"
        QT_MOC_LITERAL(516, 23),  // "UpdateFeedbackSpeed2rpm"
        QT_MOC_LITERAL(540, 23),  // "on_pushButton_5_clicked"
        QT_MOC_LITERAL(564, 19),  // "on_synStart_clicked"
        QT_MOC_LITERAL(584, 21),  // "on_speedGiven_clicked"
        QT_MOC_LITERAL(606, 24),  // "on_pushButton_10_clicked"
        QT_MOC_LITERAL(631, 23),  // "on_PosiLoopInit_clicked"
        QT_MOC_LITERAL(655, 21),  // "on_StorePMSM2_clicked"
        QT_MOC_LITERAL(677, 21),  // "on_StorePMSM1_clicked"
        QT_MOC_LITERAL(699, 18),  // "on_readCAS_clicked"
        QT_MOC_LITERAL(718, 29),  // "on_PMSM1workModeSetup_clicked"
        QT_MOC_LITERAL(748, 29),  // "on_PMSM2workModeSetup_clicked"
        QT_MOC_LITERAL(778, 27),  // "on_PosiLoopSyncInit_clicked"
        QT_MOC_LITERAL(806, 25)   // "on_PosiLoopInit_2_clicked"
    },
    "MainWindow",
    "timerCTRSend",
    "",
    "presetTimerStatus",
    "ethernetCloseCTRSend",
    "sendRefSineSig",
    "sendCnt",
    "onTimeout",
    "recvCurTimeStamp_Ms",
    "on_timerSet_clicked",
    "pageMsgRefresh",
    "realtimeDataSlot",
    "CustomPlotMousePress",
    "QMouseEvent*",
    "event",
    "CustomPlotSelectionChanged",
    "on_synStop_clicked",
    "upreupdateJudge",
    "sendNo",
    "feedbackData",
    "sampleData",
    "rttDelayUpdate",
    "updateRealTimeStatus",
    "CASREPORTFRAME",
    "statusData",
    "updateSDRAMDataSlot",
    "currentSubPackNum",
    "totalPackNum",
    "writeNum",
    "SUBPACK*",
    "array",
    "readyToSDRAMTransSlot",
    "UpdateFeedbackSpeed1rpm",
    "speedUpdateFormat",
    "curSpeedTime",
    "UpdateFeedbackSpeed2rpm",
    "on_pushButton_5_clicked",
    "on_synStart_clicked",
    "on_speedGiven_clicked",
    "on_pushButton_10_clicked",
    "on_PosiLoopInit_clicked",
    "on_StorePMSM2_clicked",
    "on_StorePMSM1_clicked",
    "on_readCAS_clicked",
    "on_PMSM1workModeSetup_clicked",
    "on_PMSM2workModeSetup_clicked",
    "on_PosiLoopSyncInit_clicked",
    "on_PosiLoopInit_2_clicked"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMainWindowENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      29,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  188,    2, 0x06,    1 /* Public */,
       4,    0,  191,    2, 0x06,    3 /* Public */,
       5,    1,  192,    2, 0x06,    4 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       7,    1,  195,    2, 0x08,    6 /* Private */,
       9,    0,  198,    2, 0x08,    8 /* Private */,
      10,    0,  199,    2, 0x08,    9 /* Private */,
      11,    0,  200,    2, 0x08,   10 /* Private */,
      12,    1,  201,    2, 0x08,   11 /* Private */,
      15,    0,  204,    2, 0x08,   13 /* Private */,
      16,    0,  205,    2, 0x08,   14 /* Private */,
      17,    2,  206,    2, 0x08,   15 /* Private */,
      21,    2,  211,    2, 0x08,   18 /* Private */,
      22,    2,  216,    2, 0x08,   21 /* Private */,
      25,    5,  221,    2, 0x08,   24 /* Private */,
      31,    1,  232,    2, 0x08,   30 /* Private */,
      32,    1,  235,    2, 0x08,   32 /* Private */,
      35,    1,  238,    2, 0x08,   34 /* Private */,
      36,    0,  241,    2, 0x08,   36 /* Private */,
      37,    0,  242,    2, 0x08,   37 /* Private */,
      38,    0,  243,    2, 0x08,   38 /* Private */,
      39,    0,  244,    2, 0x08,   39 /* Private */,
      40,    0,  245,    2, 0x08,   40 /* Private */,
      41,    0,  246,    2, 0x08,   41 /* Private */,
      42,    0,  247,    2, 0x08,   42 /* Private */,
      43,    0,  248,    2, 0x08,   43 /* Private */,
      44,    0,  249,    2, 0x08,   44 /* Private */,
      45,    0,  250,    2, 0x08,   45 /* Private */,
      46,    0,  251,    2, 0x08,   46 /* Private */,
      47,    0,  252,    2, 0x08,   47 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::UChar,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,

 // slots: parameters
    QMetaType::Void, QMetaType::UInt,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UChar, 0x80000000 | 19,   18,   20,
    QMetaType::Void, QMetaType::UChar, 0x80000000 | 19,   18,   20,
    QMetaType::Void, QMetaType::UChar, 0x80000000 | 23,   18,   24,
    QMetaType::Void, QMetaType::UChar, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt, 0x80000000 | 29,   18,   26,   27,   28,   30,
    QMetaType::Void, QMetaType::UChar,   18,
    QMetaType::Void, 0x80000000 | 33,   34,
    QMetaType::Void, 0x80000000 | 33,   34,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSMainWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMainWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMainWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'timerCTRSend'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<unsigned char, std::false_type>,
        // method 'ethernetCloseCTRSend'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sendRefSineSig'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onTimeout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<unsigned int, std::false_type>,
        // method 'on_timerSet_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'pageMsgRefresh'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'realtimeDataSlot'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'CustomPlotMousePress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QMouseEvent *, std::false_type>,
        // method 'CustomPlotSelectionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_synStop_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'upreupdateJudge'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<unsigned char, std::false_type>,
        QtPrivate::TypeAndForceComplete<feedbackData, std::false_type>,
        // method 'rttDelayUpdate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<unsigned char, std::false_type>,
        QtPrivate::TypeAndForceComplete<feedbackData, std::false_type>,
        // method 'updateRealTimeStatus'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<unsigned char, std::false_type>,
        QtPrivate::TypeAndForceComplete<CASREPORTFRAME, std::false_type>,
        // method 'updateSDRAMDataSlot'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<unsigned char, std::false_type>,
        QtPrivate::TypeAndForceComplete<unsigned int, std::false_type>,
        QtPrivate::TypeAndForceComplete<unsigned int, std::false_type>,
        QtPrivate::TypeAndForceComplete<unsigned int, std::false_type>,
        QtPrivate::TypeAndForceComplete<SUBPACK *, std::false_type>,
        // method 'readyToSDRAMTransSlot'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<unsigned char, std::false_type>,
        // method 'UpdateFeedbackSpeed1rpm'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<speedUpdateFormat, std::false_type>,
        // method 'UpdateFeedbackSpeed2rpm'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<speedUpdateFormat, std::false_type>,
        // method 'on_pushButton_5_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_synStart_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_speedGiven_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pushButton_10_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_PosiLoopInit_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_StorePMSM2_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_StorePMSM1_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_readCAS_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_PMSM1workModeSetup_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_PMSM2workModeSetup_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_PosiLoopSyncInit_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_PosiLoopInit_2_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->timerCTRSend((*reinterpret_cast< std::add_pointer_t<uchar>>(_a[1]))); break;
        case 1: _t->ethernetCloseCTRSend(); break;
        case 2: _t->sendRefSineSig((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->onTimeout((*reinterpret_cast< std::add_pointer_t<uint>>(_a[1]))); break;
        case 4: _t->on_timerSet_clicked(); break;
        case 5: _t->pageMsgRefresh(); break;
        case 6: _t->realtimeDataSlot(); break;
        case 7: _t->CustomPlotMousePress((*reinterpret_cast< std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 8: _t->CustomPlotSelectionChanged(); break;
        case 9: _t->on_synStop_clicked(); break;
        case 10: _t->upreupdateJudge((*reinterpret_cast< std::add_pointer_t<uchar>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<feedbackData>>(_a[2]))); break;
        case 11: _t->rttDelayUpdate((*reinterpret_cast< std::add_pointer_t<uchar>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<feedbackData>>(_a[2]))); break;
        case 12: _t->updateRealTimeStatus((*reinterpret_cast< std::add_pointer_t<uchar>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<CASREPORTFRAME>>(_a[2]))); break;
        case 13: _t->updateSDRAMDataSlot((*reinterpret_cast< std::add_pointer_t<uchar>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<uint>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<uint>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<SUBPACK*>>(_a[5]))); break;
        case 14: _t->readyToSDRAMTransSlot((*reinterpret_cast< std::add_pointer_t<uchar>>(_a[1]))); break;
        case 15: _t->UpdateFeedbackSpeed1rpm((*reinterpret_cast< std::add_pointer_t<speedUpdateFormat>>(_a[1]))); break;
        case 16: _t->UpdateFeedbackSpeed2rpm((*reinterpret_cast< std::add_pointer_t<speedUpdateFormat>>(_a[1]))); break;
        case 17: _t->on_pushButton_5_clicked(); break;
        case 18: _t->on_synStart_clicked(); break;
        case 19: _t->on_speedGiven_clicked(); break;
        case 20: _t->on_pushButton_10_clicked(); break;
        case 21: _t->on_PosiLoopInit_clicked(); break;
        case 22: _t->on_StorePMSM2_clicked(); break;
        case 23: _t->on_StorePMSM1_clicked(); break;
        case 24: _t->on_readCAS_clicked(); break;
        case 25: _t->on_PMSM1workModeSetup_clicked(); break;
        case 26: _t->on_PMSM2workModeSetup_clicked(); break;
        case 27: _t->on_PosiLoopSyncInit_clicked(); break;
        case 28: _t->on_PosiLoopInit_2_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< feedbackData >(); break;
            }
            break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< feedbackData >(); break;
            }
            break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< CASREPORTFRAME >(); break;
            }
            break;
        case 15:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< speedUpdateFormat >(); break;
            }
            break;
        case 16:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< speedUpdateFormat >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MainWindow::*)(unsigned char );
            if (_t _q_method = &MainWindow::timerCTRSend; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)();
            if (_t _q_method = &MainWindow::ethernetCloseCTRSend; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)(int );
            if (_t _q_method = &MainWindow::sendRefSineSig; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMainWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::timerCTRSend(unsigned char _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MainWindow::ethernetCloseCTRSend()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void MainWindow::sendRefSineSig(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
