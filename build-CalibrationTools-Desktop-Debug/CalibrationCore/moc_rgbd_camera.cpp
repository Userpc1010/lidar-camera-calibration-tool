/****************************************************************************
** Meta object code from reading C++ file 'rgbd_camera.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../CalibrationTools/CalibrationCore/rgbd_camera.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rgbd_camera.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Processor_t {
    QByteArrayData data[15];
    char stringdata0[197];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Processor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Processor_t qt_meta_stringdata_Processor = {
    {
QT_MOC_LITERAL(0, 0, 9), // "Processor"
QT_MOC_LITERAL(1, 10, 23), // "DisplayCameraPointCloud"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 8), // "GLfloat*"
QT_MOC_LITERAL(4, 44, 8), // "vertices"
QT_MOC_LITERAL(5, 53, 6), // "colors"
QT_MOC_LITERAL(6, 60, 5), // "count"
QT_MOC_LITERAL(7, 66, 22), // "DisplayLidarPointCloud"
QT_MOC_LITERAL(8, 89, 18), // "calibrationUpdated"
QT_MOC_LITERAL(9, 108, 17), // "CalibrationParams"
QT_MOC_LITERAL(10, 126, 6), // "params"
QT_MOC_LITERAL(11, 133, 21), // "DisplayCapturedClouds"
QT_MOC_LITERAL(12, 155, 28), // "std::vector<Eigen::Vector3f>"
QT_MOC_LITERAL(13, 184, 5), // "lidar"
QT_MOC_LITERAL(14, 190, 6) // "camera"

    },
    "Processor\0DisplayCameraPointCloud\0\0"
    "GLfloat*\0vertices\0colors\0count\0"
    "DisplayLidarPointCloud\0calibrationUpdated\0"
    "CalibrationParams\0params\0DisplayCapturedClouds\0"
    "std::vector<Eigen::Vector3f>\0lidar\0"
    "camera"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Processor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   34,    2, 0x06 /* Public */,
       7,    3,   41,    2, 0x06 /* Public */,
       8,    1,   48,    2, 0x06 /* Public */,
      11,    2,   51,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3, QMetaType::ULongLong,    4,    5,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3, QMetaType::ULongLong,    4,    5,    6,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 12,   13,   14,

       0        // eod
};

void Processor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Processor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->DisplayCameraPointCloud((*reinterpret_cast< GLfloat*(*)>(_a[1])),(*reinterpret_cast< GLfloat*(*)>(_a[2])),(*reinterpret_cast< unsigned long long(*)>(_a[3]))); break;
        case 1: _t->DisplayLidarPointCloud((*reinterpret_cast< GLfloat*(*)>(_a[1])),(*reinterpret_cast< GLfloat*(*)>(_a[2])),(*reinterpret_cast< unsigned long long(*)>(_a[3]))); break;
        case 2: _t->calibrationUpdated((*reinterpret_cast< const CalibrationParams(*)>(_a[1]))); break;
        case 3: _t->DisplayCapturedClouds((*reinterpret_cast< const std::vector<Eigen::Vector3f>(*)>(_a[1])),(*reinterpret_cast< const std::vector<Eigen::Vector3f>(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Processor::*)(GLfloat * , GLfloat * , unsigned long long );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Processor::DisplayCameraPointCloud)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Processor::*)(GLfloat * , GLfloat * , unsigned long long );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Processor::DisplayLidarPointCloud)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Processor::*)(const CalibrationParams & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Processor::calibrationUpdated)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Processor::*)(const std::vector<Eigen::Vector3f> & , const std::vector<Eigen::Vector3f> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Processor::DisplayCapturedClouds)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Processor::staticMetaObject = { {
    &QThread::staticMetaObject,
    qt_meta_stringdata_Processor.data,
    qt_meta_data_Processor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Processor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Processor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Processor.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int Processor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
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
void Processor::DisplayCameraPointCloud(GLfloat * _t1, GLfloat * _t2, unsigned long long _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Processor::DisplayLidarPointCloud(GLfloat * _t1, GLfloat * _t2, unsigned long long _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Processor::calibrationUpdated(const CalibrationParams & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Processor::DisplayCapturedClouds(const std::vector<Eigen::Vector3f> & _t1, const std::vector<Eigen::Vector3f> & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
