/****************************************************************************
** Meta object code from reading C++ file 'services_list.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../clients/client-ui/qt/include/services_list.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'services_list.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ServicesList_t {
    QByteArrayData data[16];
    char stringdata0[221];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ServicesList_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ServicesList_t qt_meta_stringdata_ServicesList = {
    {
QT_MOC_LITERAL(0, 0, 12), // "ServicesList"
QT_MOC_LITERAL(1, 13, 13), // "SelectService"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 11), // "const char*"
QT_MOC_LITERAL(4, 40, 14), // "service_name_s"
QT_MOC_LITERAL(5, 55, 13), // "const json_t*"
QT_MOC_LITERAL(6, 69, 13), // "params_json_p"
QT_MOC_LITERAL(7, 83, 22), // "ToggleServiceRunStatus"
QT_MOC_LITERAL(8, 106, 7), // "index_r"
QT_MOC_LITERAL(9, 114, 21), // "CheckServiceRunStatus"
QT_MOC_LITERAL(10, 136, 22), // "const QListWidgetItem*"
QT_MOC_LITERAL(11, 159, 6), // "item_p"
QT_MOC_LITERAL(12, 166, 17), // "SetCurrentService"
QT_MOC_LITERAL(13, 184, 19), // "SetServiceRunStatus"
QT_MOC_LITERAL(14, 204, 10), // "char*const"
QT_MOC_LITERAL(15, 215, 5) // "state"

    },
    "ServicesList\0SelectService\0\0const char*\0"
    "service_name_s\0const json_t*\0params_json_p\0"
    "ToggleServiceRunStatus\0index_r\0"
    "CheckServiceRunStatus\0const QListWidgetItem*\0"
    "item_p\0SetCurrentService\0SetServiceRunStatus\0"
    "char*const\0state"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ServicesList[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x0a /* Public */,
       7,    1,   44,    2, 0x08 /* Private */,
       9,    1,   47,    2, 0x08 /* Private */,
      12,    1,   50,    2, 0x08 /* Private */,
      13,    2,   53,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, QMetaType::QModelIndex,    8,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, QMetaType::QModelIndex,    8,
    QMetaType::Void, 0x80000000 | 14, QMetaType::Bool,    4,   15,

       0        // eod
};

void ServicesList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ServicesList *_t = static_cast<ServicesList *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->SelectService((*reinterpret_cast< const char*(*)>(_a[1])),(*reinterpret_cast< const json_t*(*)>(_a[2]))); break;
        case 1: _t->ToggleServiceRunStatus((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 2: _t->CheckServiceRunStatus((*reinterpret_cast< const QListWidgetItem*(*)>(_a[1]))); break;
        case 3: _t->SetCurrentService((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 4: _t->SetServiceRunStatus((*reinterpret_cast< const char*const(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject ServicesList::staticMetaObject = {
    { &QListWidget::staticMetaObject, qt_meta_stringdata_ServicesList.data,
      qt_meta_data_ServicesList,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ServicesList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ServicesList::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ServicesList.stringdata0))
        return static_cast<void*>(const_cast< ServicesList*>(this));
    if (!strcmp(_clname, "ServiceUI"))
        return static_cast< ServiceUI*>(const_cast< ServicesList*>(this));
    return QListWidget::qt_metacast(_clname);
}

int ServicesList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListWidget::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
