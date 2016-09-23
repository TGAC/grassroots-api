/****************************************************************************
** Meta object code from reading C++ file 'param_check_box.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../clients/client-ui/qt/include/param_check_box.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'param_check_box.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ParamCheckBox_t {
    QByteArrayData data[4];
    char stringdata0[34];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ParamCheckBox_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ParamCheckBox_t qt_meta_stringdata_ParamCheckBox = {
    {
QT_MOC_LITERAL(0, 0, 13), // "ParamCheckBox"
QT_MOC_LITERAL(1, 14, 12), // "UpdateConfig"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 5) // "state"

    },
    "ParamCheckBox\0UpdateConfig\0\0state"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ParamCheckBox[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Bool, QMetaType::Int,    3,

       0        // eod
};

void ParamCheckBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ParamCheckBox *_t = static_cast<ParamCheckBox *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: { bool _r = _t->UpdateConfig((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObject ParamCheckBox::staticMetaObject = {
    { &BaseParamWidget::staticMetaObject, qt_meta_stringdata_ParamCheckBox.data,
      qt_meta_data_ParamCheckBox,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ParamCheckBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ParamCheckBox::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ParamCheckBox.stringdata0))
        return static_cast<void*>(const_cast< ParamCheckBox*>(this));
    return BaseParamWidget::qt_metacast(_clname);
}

int ParamCheckBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseParamWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
