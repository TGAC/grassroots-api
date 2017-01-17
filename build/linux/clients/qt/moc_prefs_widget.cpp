/****************************************************************************
** Meta object code from reading C++ file 'prefs_widget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../clients/client-ui/qt/include/prefs_widget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'prefs_widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PrefsWidget_t {
    QByteArrayData data[8];
    char stringdata0[95];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PrefsWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PrefsWidget_t qt_meta_stringdata_PrefsWidget = {
    {
QT_MOC_LITERAL(0, 0, 11), // "PrefsWidget"
QT_MOC_LITERAL(1, 12, 21), // "InterfaceLevelChanged"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 14), // "ParameterLevel"
QT_MOC_LITERAL(4, 50, 5), // "level"
QT_MOC_LITERAL(5, 56, 11), // "RunServices"
QT_MOC_LITERAL(6, 68, 8), // "run_flag"
QT_MOC_LITERAL(7, 77, 17) // "SetInterfaceLevel"

    },
    "PrefsWidget\0InterfaceLevelChanged\0\0"
    "ParameterLevel\0level\0RunServices\0"
    "run_flag\0SetInterfaceLevel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PrefsWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       5,    1,   32,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,   35,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Bool,    6,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

void PrefsWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PrefsWidget *_t = static_cast<PrefsWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->InterfaceLevelChanged((*reinterpret_cast< ParameterLevel(*)>(_a[1]))); break;
        case 1: _t->RunServices((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->SetInterfaceLevel((*reinterpret_cast< ParameterLevel(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PrefsWidget::*_t)(ParameterLevel );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PrefsWidget::InterfaceLevelChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (PrefsWidget::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PrefsWidget::RunServices)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject PrefsWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_PrefsWidget.data,
      qt_meta_data_PrefsWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PrefsWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PrefsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PrefsWidget.stringdata0))
        return static_cast<void*>(const_cast< PrefsWidget*>(this));
    if (!strcmp(_clname, "RunnableWidget"))
        return static_cast< RunnableWidget*>(const_cast< PrefsWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int PrefsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void PrefsWidget::InterfaceLevelChanged(ParameterLevel _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PrefsWidget::RunServices(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
