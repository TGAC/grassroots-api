/****************************************************************************
** Meta object code from reading C++ file 'json_viewer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../clients/client-ui/qt/include/json_viewer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'json_viewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_JSONViewer_t {
    QByteArrayData data[13];
    char stringdata0[145];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_JSONViewer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_JSONViewer_t qt_meta_stringdata_JSONViewer = {
    {
QT_MOC_LITERAL(0, 0, 10), // "JSONViewer"
QT_MOC_LITERAL(1, 11, 19), // "RunServiceRequested"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 7), // "json_t*"
QT_MOC_LITERAL(4, 40, 9), // "request_p"
QT_MOC_LITERAL(5, 50, 11), // "PrepareMenu"
QT_MOC_LITERAL(6, 62, 5), // "pos_r"
QT_MOC_LITERAL(7, 68, 16), // "RunLinkedService"
QT_MOC_LITERAL(8, 85, 12), // "checked_flag"
QT_MOC_LITERAL(9, 98, 13), // "ResizeColumns"
QT_MOC_LITERAL(10, 112, 5), // "index"
QT_MOC_LITERAL(11, 118, 13), // "SetSystemFont"
QT_MOC_LITERAL(12, 132, 12) // "SetFixedFont"

    },
    "JSONViewer\0RunServiceRequested\0\0json_t*\0"
    "request_p\0PrepareMenu\0pos_r\0"
    "RunLinkedService\0checked_flag\0"
    "ResizeColumns\0index\0SetSystemFont\0"
    "SetFixedFont"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_JSONViewer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   47,    2, 0x08 /* Private */,
       7,    1,   50,    2, 0x08 /* Private */,
       9,    1,   53,    2, 0x08 /* Private */,
      11,    0,   56,    2, 0x08 /* Private */,
      12,    0,   57,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, QMetaType::QPoint,    6,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::QModelIndex,   10,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void JSONViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        JSONViewer *_t = static_cast<JSONViewer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->RunServiceRequested((*reinterpret_cast< json_t*(*)>(_a[1]))); break;
        case 1: _t->PrepareMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 2: _t->RunLinkedService((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->ResizeColumns((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 4: _t->SetSystemFont(); break;
        case 5: _t->SetFixedFont(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (JSONViewer::*_t)(json_t * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JSONViewer::RunServiceRequested)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject JSONViewer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_JSONViewer.data,
      qt_meta_data_JSONViewer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *JSONViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *JSONViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_JSONViewer.stringdata0))
        return static_cast<void*>(const_cast< JSONViewer*>(this));
    if (!strcmp(_clname, "ViewableWidget"))
        return static_cast< ViewableWidget*>(const_cast< JSONViewer*>(this));
    return QWidget::qt_metacast(_clname);
}

int JSONViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void JSONViewer::RunServiceRequested(json_t * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
