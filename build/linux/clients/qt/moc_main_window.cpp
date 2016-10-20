/****************************************************************************
** Meta object code from reading C++ file 'main_window.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../clients/client-ui/qt/include/main_window.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'main_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[25];
    char stringdata0[370];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 6), // "Closed"
QT_MOC_LITERAL(2, 18, 0), // ""
QT_MOC_LITERAL(3, 19, 16), // "ServiceRequested"
QT_MOC_LITERAL(4, 36, 11), // "const char*"
QT_MOC_LITERAL(5, 48, 14), // "service_name_s"
QT_MOC_LITERAL(6, 63, 13), // "const json_t*"
QT_MOC_LITERAL(7, 77, 13), // "params_json_p"
QT_MOC_LITERAL(8, 91, 22), // "SetBasicInterfaceLevel"
QT_MOC_LITERAL(9, 114, 29), // "SetIntermediateInterfaceLevel"
QT_MOC_LITERAL(10, 144, 25), // "SetAdvancedInterfaceLevel"
QT_MOC_LITERAL(11, 170, 17), // "LoadConfiguration"
QT_MOC_LITERAL(12, 188, 17), // "SaveConfiguration"
QT_MOC_LITERAL(13, 206, 11), // "RunServices"
QT_MOC_LITERAL(14, 218, 8), // "run_flag"
QT_MOC_LITERAL(15, 227, 16), // "RunKeywordSearch"
QT_MOC_LITERAL(16, 244, 8), // "keywords"
QT_MOC_LITERAL(17, 253, 13), // "SelectService"
QT_MOC_LITERAL(18, 267, 14), // "GetAllServices"
QT_MOC_LITERAL(19, 282, 21), // "GetInterestedServices"
QT_MOC_LITERAL(20, 304, 18), // "RunKeywordServices"
QT_MOC_LITERAL(21, 323, 16), // "GetNamedServices"
QT_MOC_LITERAL(22, 340, 15), // "ConnectToServer"
QT_MOC_LITERAL(23, 356, 6), // "Accept"
QT_MOC_LITERAL(24, 363, 6) // "Reject"

    },
    "MainWindow\0Closed\0\0ServiceRequested\0"
    "const char*\0service_name_s\0const json_t*\0"
    "params_json_p\0SetBasicInterfaceLevel\0"
    "SetIntermediateInterfaceLevel\0"
    "SetAdvancedInterfaceLevel\0LoadConfiguration\0"
    "SaveConfiguration\0RunServices\0run_flag\0"
    "RunKeywordSearch\0keywords\0SelectService\0"
    "GetAllServices\0GetInterestedServices\0"
    "RunKeywordServices\0GetNamedServices\0"
    "ConnectToServer\0Accept\0Reject"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   99,    2, 0x06 /* Public */,
       3,    2,  100,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    0,  105,    2, 0x0a /* Public */,
       9,    0,  106,    2, 0x0a /* Public */,
      10,    0,  107,    2, 0x0a /* Public */,
      11,    0,  108,    2, 0x0a /* Public */,
      12,    0,  109,    2, 0x0a /* Public */,
      13,    1,  110,    2, 0x0a /* Public */,
      15,    1,  113,    2, 0x0a /* Public */,
      17,    2,  116,    2, 0x0a /* Public */,
      18,    0,  121,    2, 0x0a /* Public */,
      19,    0,  122,    2, 0x0a /* Public */,
      20,    0,  123,    2, 0x0a /* Public */,
      21,    0,  124,    2, 0x0a /* Public */,
      22,    0,  125,    2, 0x0a /* Public */,
      23,    0,  126,    2, 0x08 /* Private */,
      24,    0,  127,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 6,    5,    7,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   14,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 6,    5,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Closed(); break;
        case 1: _t->ServiceRequested((*reinterpret_cast< const char*(*)>(_a[1])),(*reinterpret_cast< const json_t*(*)>(_a[2]))); break;
        case 2: _t->SetBasicInterfaceLevel(); break;
        case 3: _t->SetIntermediateInterfaceLevel(); break;
        case 4: _t->SetAdvancedInterfaceLevel(); break;
        case 5: _t->LoadConfiguration(); break;
        case 6: _t->SaveConfiguration(); break;
        case 7: _t->RunServices((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->RunKeywordSearch((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: _t->SelectService((*reinterpret_cast< const char*(*)>(_a[1])),(*reinterpret_cast< const json_t*(*)>(_a[2]))); break;
        case 10: _t->GetAllServices(); break;
        case 11: _t->GetInterestedServices(); break;
        case 12: _t->RunKeywordServices(); break;
        case 13: _t->GetNamedServices(); break;
        case 14: _t->ConnectToServer(); break;
        case 15: _t->Accept(); break;
        case 16: _t->Reject(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MainWindow::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::Closed)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (MainWindow::*_t)(const char * , const json_t * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::ServiceRequested)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::Closed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void MainWindow::ServiceRequested(const char * _t1, const json_t * _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
