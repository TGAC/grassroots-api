/****************************************************************************
** Meta object code from reading C++ file 'service_prefs_widget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/client-ui/qt/include/service_prefs_widget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'service_prefs_widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ServicePrefsWidget_t {
    QByteArrayData data[9];
    char stringdata0[101];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ServicePrefsWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ServicePrefsWidget_t qt_meta_stringdata_ServicePrefsWidget = {
    {
QT_MOC_LITERAL(0, 0, 18), // "ServicePrefsWidget"
QT_MOC_LITERAL(1, 19, 16), // "RunStatusChanged"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 10), // "char*const"
QT_MOC_LITERAL(4, 48, 14), // "service_name_s"
QT_MOC_LITERAL(5, 63, 6), // "status"
QT_MOC_LITERAL(6, 70, 10), // "SetRunFlag"
QT_MOC_LITERAL(7, 81, 5), // "state"
QT_MOC_LITERAL(8, 87, 13) // "ToggleRunFlag"

    },
    "ServicePrefsWidget\0RunStatusChanged\0"
    "\0char*const\0service_name_s\0status\0"
    "SetRunFlag\0state\0ToggleRunFlag"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ServicePrefsWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   34,    2, 0x0a /* Public */,
       8,    0,   37,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Bool,    4,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void,

       0        // eod
};

void ServicePrefsWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ServicePrefsWidget *_t = static_cast<ServicePrefsWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->RunStatusChanged((*reinterpret_cast< const char*const(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->SetRunFlag((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->ToggleRunFlag(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ServicePrefsWidget::*_t)(const char * const , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ServicePrefsWidget::RunStatusChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject ServicePrefsWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ServicePrefsWidget.data,
      qt_meta_data_ServicePrefsWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ServicePrefsWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ServicePrefsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ServicePrefsWidget.stringdata0))
        return static_cast<void*>(const_cast< ServicePrefsWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int ServicePrefsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void ServicePrefsWidget::RunStatusChanged(const char * const _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
