/****************************************************************************
** Meta object code from reading C++ file 'cthread.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../sample/linux/qt-demo/cthread.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CThread[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,    9,    8,    8, 0x05,
      51,   48,    8,    8, 0x05,
      79,    9,    8,    8, 0x05,
     124,  114,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
     169,    8,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CThread[] = {
    "CThread\0\0id,fd\0send_fData(int,HUST::FlightData)\0"
    "fd\0send_info(HUST::FlightData)\0"
    "send_error_info(int,HUST::Positon)\0"
    "id,fd,max\0send_max_error_info(int,HUST::Positon,float)\0"
    "stopImmediately()\0"
};

void CThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CThread *_t = static_cast<CThread *>(_o);
        switch (_id) {
        case 0: _t->send_fData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< HUST::FlightData(*)>(_a[2]))); break;
        case 1: _t->send_info((*reinterpret_cast< HUST::FlightData(*)>(_a[1]))); break;
        case 2: _t->send_error_info((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< HUST::Positon(*)>(_a[2]))); break;
        case 3: _t->send_max_error_info((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< HUST::Positon(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3]))); break;
        case 4: _t->stopImmediately(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CThread::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_CThread,
      qt_meta_data_CThread, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CThread))
        return static_cast<void*>(const_cast< CThread*>(this));
    return QThread::qt_metacast(_clname);
}

int CThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void CThread::send_fData(int _t1, HUST::FlightData _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CThread::send_info(HUST::FlightData _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CThread::send_error_info(int _t1, HUST::Positon _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CThread::send_max_error_info(int _t1, HUST::Positon _t2, float _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
