/****************************************************************************
** Meta object code from reading C++ file 'results_widget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../clients/client-ui/qt/include/results_widget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'results_widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ResultsWidget_t {
    QByteArrayData data[12];
    char stringdata0[155];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ResultsWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ResultsWidget_t qt_meta_stringdata_ResultsWidget = {
    {
QT_MOC_LITERAL(0, 0, 13), // "ResultsWidget"
QT_MOC_LITERAL(1, 14, 16), // "ServiceRequested"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 11), // "const char*"
QT_MOC_LITERAL(4, 44, 14), // "service_name_s"
QT_MOC_LITERAL(5, 59, 13), // "const json_t*"
QT_MOC_LITERAL(6, 73, 13), // "params_json_p"
QT_MOC_LITERAL(7, 87, 19), // "RunServiceRequested"
QT_MOC_LITERAL(8, 107, 7), // "json_t*"
QT_MOC_LITERAL(9, 115, 14), // "service_json_p"
QT_MOC_LITERAL(10, 130, 13), // "SelectService"
QT_MOC_LITERAL(11, 144, 10) // "RunService"

    },
    "ResultsWidget\0ServiceRequested\0\0"
    "const char*\0service_name_s\0const json_t*\0"
    "params_json_p\0RunServiceRequested\0"
    "json_t*\0service_json_p\0SelectService\0"
    "RunService"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ResultsWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   34,    2, 0x06 /* Public */,
       7,    1,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    2,   42,    2, 0x0a /* Public */,
      11,    1,   47,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 8,    9,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 8,    9,

       0        // eod
};

void ResultsWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ResultsWidget *_t = static_cast<ResultsWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ServiceRequested((*reinterpret_cast< const char*(*)>(_a[1])),(*reinterpret_cast< const json_t*(*)>(_a[2]))); break;
        case 1: _t->RunServiceRequested((*reinterpret_cast< json_t*(*)>(_a[1]))); break;
        case 2: _t->SelectService((*reinterpret_cast< const char*(*)>(_a[1])),(*reinterpret_cast< const json_t*(*)>(_a[2]))); break;
        case 3: _t->RunService((*reinterpret_cast< json_t*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ResultsWidget::*_t)(const char * , const json_t * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ResultsWidget::ServiceRequested)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (ResultsWidget::*_t)(json_t * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ResultsWidget::RunServiceRequested)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject ResultsWidget::staticMetaObject = {
    { &QTabWidget::staticMetaObject, qt_meta_stringdata_ResultsWidget.data,
      qt_meta_data_ResultsWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ResultsWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ResultsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ResultsWidget.stringdata0))
        return static_cast<void*>(const_cast< ResultsWidget*>(this));
    return QTabWidget::qt_metacast(_clname);
}

int ResultsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void ResultsWidget::ServiceRequested(const char * _t1, const json_t * _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ResultsWidget::RunServiceRequested(json_t * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
