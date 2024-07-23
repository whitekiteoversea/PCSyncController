/****************************************************************************
** Meta object code from reading C++ file 'timemonitor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../timemonitor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'timemonitor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_timeMonitor_t {
    QByteArrayData data[11];
    char stringdata0[146];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_timeMonitor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_timeMonitor_t qt_meta_stringdata_timeMonitor = {
    {
QT_MOC_LITERAL(0, 0, 11), // "timeMonitor"
QT_MOC_LITERAL(1, 12, 18), // "cur_TimestampPrint"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 10), // "curTime_Ms"
QT_MOC_LITERAL(4, 43, 13), // "timerCloseSig"
QT_MOC_LITERAL(5, 57, 13), // "timerStartSig"
QT_MOC_LITERAL(6, 71, 14), // "timingNowStart"
QT_MOC_LITERAL(7, 86, 13), // "timingNowStop"
QT_MOC_LITERAL(8, 100, 20), // "timerStatusOperation"
QT_MOC_LITERAL(9, 121, 12), // "OperationCmd"
QT_MOC_LITERAL(10, 134, 11) // "handleEvent"

    },
    "timeMonitor\0cur_TimestampPrint\0\0"
    "curTime_Ms\0timerCloseSig\0timerStartSig\0"
    "timingNowStart\0timingNowStop\0"
    "timerStatusOperation\0OperationCmd\0"
    "handleEvent"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_timeMonitor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       4,    0,   52,    2, 0x06 /* Public */,
       5,    0,   53,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   54,    2, 0x0a /* Public */,
       7,    0,   55,    2, 0x0a /* Public */,
       8,    1,   56,    2, 0x0a /* Public */,
      10,    0,   59,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::UInt,    3,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UChar,    9,
    QMetaType::Void,

       0        // eod
};

void timeMonitor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<timeMonitor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->cur_TimestampPrint((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 1: _t->timerCloseSig(); break;
        case 2: _t->timerStartSig(); break;
        case 3: _t->timingNowStart(); break;
        case 4: _t->timingNowStop(); break;
        case 5: _t->timerStatusOperation((*reinterpret_cast< unsigned char(*)>(_a[1]))); break;
        case 6: _t->handleEvent(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (timeMonitor::*)(unsigned int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&timeMonitor::cur_TimestampPrint)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (timeMonitor::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&timeMonitor::timerCloseSig)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (timeMonitor::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&timeMonitor::timerStartSig)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject timeMonitor::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_timeMonitor.data,
    qt_meta_data_timeMonitor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *timeMonitor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *timeMonitor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_timeMonitor.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int timeMonitor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void timeMonitor::cur_TimestampPrint(unsigned int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void timeMonitor::timerCloseSig()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void timeMonitor::timerStartSig()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
