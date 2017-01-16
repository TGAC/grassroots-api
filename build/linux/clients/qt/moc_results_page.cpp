/****************************************************************************
** Meta object code from reading C++ file 'results_page.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../clients/client-ui/qt/include/results_page.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'results_page.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ResultsPage_t {
    QByteArrayData data[15];
    char stringdata0[177];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ResultsPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ResultsPage_t qt_meta_stringdata_ResultsPage = {
    {
QT_MOC_LITERAL(0, 0, 11), // "ResultsPage"
QT_MOC_LITERAL(1, 12, 16), // "ServiceRequested"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 11), // "const char*"
QT_MOC_LITERAL(4, 42, 14), // "service_name_s"
QT_MOC_LITERAL(5, 57, 13), // "const json_t*"
QT_MOC_LITERAL(6, 71, 13), // "params_json_p"
QT_MOC_LITERAL(7, 85, 19), // "RunServiceRequested"
QT_MOC_LITERAL(8, 105, 7), // "json_t*"
QT_MOC_LITERAL(9, 113, 9), // "request_p"
QT_MOC_LITERAL(10, 123, 11), // "OpenWebLink"
QT_MOC_LITERAL(11, 135, 10), // "char*const"
QT_MOC_LITERAL(12, 146, 5), // "uri_s"
QT_MOC_LITERAL(13, 152, 13), // "SelectService"
QT_MOC_LITERAL(14, 166, 10) // "RunService"

    },
    "ResultsPage\0ServiceRequested\0\0const char*\0"
    "service_name_s\0const json_t*\0params_json_p\0"
    "RunServiceRequested\0json_t*\0request_p\0"
    "OpenWebLink\0char*const\0uri_s\0SelectService\0"
    "RunService"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ResultsPage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x06 /* Public */,
       7,    1,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    1,   47,    2, 0x0a /* Public */,
      13,    2,   50,    2, 0x0a /* Public */,
      14,    1,   55,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 8,    9,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 8,    9,

       0        // eod
};

void ResultsPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ResultsPage *_t = static_cast<ResultsPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ServiceRequested((*reinterpret_cast< const char*(*)>(_a[1])),(*reinterpret_cast< const json_t*(*)>(_a[2]))); break;
        case 1: _t->RunServiceRequested((*reinterpret_cast< json_t*(*)>(_a[1]))); break;
        case 2: _t->OpenWebLink((*reinterpret_cast< const char*const(*)>(_a[1]))); break;
        case 3: _t->SelectService((*reinterpret_cast< const char*(*)>(_a[1])),(*reinterpret_cast< const json_t*(*)>(_a[2]))); break;
        case 4: _t->RunService((*reinterpret_cast< json_t*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ResultsPage::*_t)(const char * , const json_t * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ResultsPage::ServiceRequested)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (ResultsPage::*_t)(json_t * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ResultsPage::RunServiceRequested)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject ResultsPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ResultsPage.data,
      qt_meta_data_ResultsPage,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ResultsPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ResultsPage::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ResultsPage.stringdata0))
        return static_cast<void*>(const_cast< ResultsPage*>(this));
    return QWidget::qt_metacast(_clname);
}

int ResultsPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void ResultsPage::ServiceRequested(const char * _t1, const json_t * _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ResultsPage::RunServiceRequested(json_t * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
