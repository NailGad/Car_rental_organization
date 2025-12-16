/****************************************************************************
** Meta object code from reading C++ file 'adminmainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ui/adminmainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'adminmainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AdminMainWindow_t {
    QByteArrayData data[12];
    char stringdata0[144];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AdminMainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AdminMainWindow_t qt_meta_stringdata_AdminMainWindow = {
    {
QT_MOC_LITERAL(0, 0, 15), // "AdminMainWindow"
QT_MOC_LITERAL(1, 16, 8), // "onAddCar"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 9), // "onEditCar"
QT_MOC_LITERAL(4, 36, 11), // "onDeleteCar"
QT_MOC_LITERAL(5, 48, 16), // "onCompleteRental"
QT_MOC_LITERAL(6, 65, 13), // "onShowReports"
QT_MOC_LITERAL(7, 79, 11), // "onShowUsers"
QT_MOC_LITERAL(8, 91, 12), // "onDeleteUser"
QT_MOC_LITERAL(9, 104, 14), // "onImportExport"
QT_MOC_LITERAL(10, 119, 8), // "onLogout"
QT_MOC_LITERAL(11, 128, 15) // "onSearchRentals"

    },
    "AdminMainWindow\0onAddCar\0\0onEditCar\0"
    "onDeleteCar\0onCompleteRental\0onShowReports\0"
    "onShowUsers\0onDeleteUser\0onImportExport\0"
    "onLogout\0onSearchRentals"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AdminMainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x08 /* Private */,
       3,    0,   65,    2, 0x08 /* Private */,
       4,    0,   66,    2, 0x08 /* Private */,
       5,    0,   67,    2, 0x08 /* Private */,
       6,    0,   68,    2, 0x08 /* Private */,
       7,    0,   69,    2, 0x08 /* Private */,
       8,    0,   70,    2, 0x08 /* Private */,
       9,    0,   71,    2, 0x08 /* Private */,
      10,    0,   72,    2, 0x08 /* Private */,
      11,    0,   73,    2, 0x08 /* Private */,

 // slots: parameters
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

void AdminMainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AdminMainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onAddCar(); break;
        case 1: _t->onEditCar(); break;
        case 2: _t->onDeleteCar(); break;
        case 3: _t->onCompleteRental(); break;
        case 4: _t->onShowReports(); break;
        case 5: _t->onShowUsers(); break;
        case 6: _t->onDeleteUser(); break;
        case 7: _t->onImportExport(); break;
        case 8: _t->onLogout(); break;
        case 9: _t->onSearchRentals(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject AdminMainWindow::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_AdminMainWindow.data,
    qt_meta_data_AdminMainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *AdminMainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AdminMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AdminMainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int AdminMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
