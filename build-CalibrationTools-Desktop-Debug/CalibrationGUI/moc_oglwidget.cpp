/****************************************************************************
** Meta object code from reading C++ file 'oglwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../CalibrationTools/CalibrationGUI/oglwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'oglwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_OGLWidget_t {
    QByteArrayData data[34];
    char stringdata0[435];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OGLWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OGLWidget_t qt_meta_stringdata_OGLWidget = {
    {
QT_MOC_LITERAL(0, 0, 9), // "OGLWidget"
QT_MOC_LITERAL(1, 10, 12), // "eulerChanged"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 4), // "roll"
QT_MOC_LITERAL(4, 29, 5), // "pitch"
QT_MOC_LITERAL(5, 35, 3), // "yaw"
QT_MOC_LITERAL(6, 39, 13), // "matrixChanged"
QT_MOC_LITERAL(7, 53, 13), // "const double*"
QT_MOC_LITERAL(8, 67, 8), // "rotation"
QT_MOC_LITERAL(9, 76, 18), // "translationChanged"
QT_MOC_LITERAL(10, 95, 11), // "translation"
QT_MOC_LITERAL(11, 107, 14), // "toEulerClicked"
QT_MOC_LITERAL(12, 122, 12), // "resetClicked"
QT_MOC_LITERAL(13, 135, 12), // "applyClicked"
QT_MOC_LITERAL(14, 148, 14), // "captureClicked"
QT_MOC_LITERAL(15, 163, 11), // "saveClicked"
QT_MOC_LITERAL(16, 175, 11), // "loadClicked"
QT_MOC_LITERAL(17, 187, 20), // "autoCalibrateClicked"
QT_MOC_LITERAL(18, 208, 11), // "cropChanged"
QT_MOC_LITERAL(19, 220, 12), // "const float*"
QT_MOC_LITERAL(20, 233, 9), // "lidar_min"
QT_MOC_LITERAL(21, 243, 9), // "lidar_max"
QT_MOC_LITERAL(22, 253, 10), // "camera_min"
QT_MOC_LITERAL(23, 264, 10), // "camera_max"
QT_MOC_LITERAL(24, 275, 14), // "presetSelected"
QT_MOC_LITERAL(25, 290, 5), // "index"
QT_MOC_LITERAL(26, 296, 22), // "saveCalibrationClicked"
QT_MOC_LITERAL(27, 319, 22), // "loadCalibrationClicked"
QT_MOC_LITERAL(28, 342, 23), // "DisplayCameraPointCloud"
QT_MOC_LITERAL(29, 366, 8), // "GLfloat*"
QT_MOC_LITERAL(30, 375, 15), // "vertices_buffer"
QT_MOC_LITERAL(31, 391, 12), // "color_buffer"
QT_MOC_LITERAL(32, 404, 7), // "counter"
QT_MOC_LITERAL(33, 412, 22) // "DisplayLidarPointCloud"

    },
    "OGLWidget\0eulerChanged\0\0roll\0pitch\0"
    "yaw\0matrixChanged\0const double*\0"
    "rotation\0translationChanged\0translation\0"
    "toEulerClicked\0resetClicked\0applyClicked\0"
    "captureClicked\0saveClicked\0loadClicked\0"
    "autoCalibrateClicked\0cropChanged\0"
    "const float*\0lidar_min\0lidar_max\0"
    "camera_min\0camera_max\0presetSelected\0"
    "index\0saveCalibrationClicked\0"
    "loadCalibrationClicked\0DisplayCameraPointCloud\0"
    "GLfloat*\0vertices_buffer\0color_buffer\0"
    "counter\0DisplayLidarPointCloud"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OGLWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      14,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   94,    2, 0x06 /* Public */,
       6,    1,  101,    2, 0x06 /* Public */,
       9,    1,  104,    2, 0x06 /* Public */,
      11,    0,  107,    2, 0x06 /* Public */,
      12,    0,  108,    2, 0x06 /* Public */,
      13,    0,  109,    2, 0x06 /* Public */,
      14,    0,  110,    2, 0x06 /* Public */,
      15,    0,  111,    2, 0x06 /* Public */,
      16,    0,  112,    2, 0x06 /* Public */,
      17,    0,  113,    2, 0x06 /* Public */,
      18,    4,  114,    2, 0x06 /* Public */,
      24,    1,  123,    2, 0x06 /* Public */,
      26,    0,  126,    2, 0x06 /* Public */,
      27,    0,  127,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      28,    3,  128,    2, 0x0a /* Public */,
      33,    3,  135,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double, QMetaType::Double, QMetaType::Double,    3,    4,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 7,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 19, 0x80000000 | 19, 0x80000000 | 19, 0x80000000 | 19,   20,   21,   22,   23,
    QMetaType::Void, QMetaType::Int,   25,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 29, 0x80000000 | 29, QMetaType::ULongLong,   30,   31,   32,
    QMetaType::Void, 0x80000000 | 29, 0x80000000 | 29, QMetaType::ULongLong,   30,   31,   32,

       0        // eod
};

void OGLWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<OGLWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->eulerChanged((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 1: _t->matrixChanged((*reinterpret_cast< const double*(*)>(_a[1]))); break;
        case 2: _t->translationChanged((*reinterpret_cast< const double*(*)>(_a[1]))); break;
        case 3: _t->toEulerClicked(); break;
        case 4: _t->resetClicked(); break;
        case 5: _t->applyClicked(); break;
        case 6: _t->captureClicked(); break;
        case 7: _t->saveClicked(); break;
        case 8: _t->loadClicked(); break;
        case 9: _t->autoCalibrateClicked(); break;
        case 10: _t->cropChanged((*reinterpret_cast< const float*(*)>(_a[1])),(*reinterpret_cast< const float*(*)>(_a[2])),(*reinterpret_cast< const float*(*)>(_a[3])),(*reinterpret_cast< const float*(*)>(_a[4]))); break;
        case 11: _t->presetSelected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->saveCalibrationClicked(); break;
        case 13: _t->loadCalibrationClicked(); break;
        case 14: _t->DisplayCameraPointCloud((*reinterpret_cast< GLfloat*(*)>(_a[1])),(*reinterpret_cast< GLfloat*(*)>(_a[2])),(*reinterpret_cast< unsigned long long(*)>(_a[3]))); break;
        case 15: _t->DisplayLidarPointCloud((*reinterpret_cast< GLfloat*(*)>(_a[1])),(*reinterpret_cast< GLfloat*(*)>(_a[2])),(*reinterpret_cast< unsigned long long(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (OGLWidget::*)(double , double , double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OGLWidget::eulerChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (OGLWidget::*)(const double * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OGLWidget::matrixChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (OGLWidget::*)(const double * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OGLWidget::translationChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (OGLWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OGLWidget::toEulerClicked)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (OGLWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OGLWidget::resetClicked)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (OGLWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OGLWidget::applyClicked)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (OGLWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OGLWidget::captureClicked)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (OGLWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OGLWidget::saveClicked)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (OGLWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OGLWidget::loadClicked)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (OGLWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OGLWidget::autoCalibrateClicked)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (OGLWidget::*)(const float * , const float * , const float * , const float * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OGLWidget::cropChanged)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (OGLWidget::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OGLWidget::presetSelected)) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (OGLWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OGLWidget::saveCalibrationClicked)) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (OGLWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OGLWidget::loadCalibrationClicked)) {
                *result = 13;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject OGLWidget::staticMetaObject = { {
    &QOpenGLWidget::staticMetaObject,
    qt_meta_stringdata_OGLWidget.data,
    qt_meta_data_OGLWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *OGLWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OGLWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OGLWidget.stringdata0))
        return static_cast<void*>(this);
    return QOpenGLWidget::qt_metacast(_clname);
}

int OGLWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QOpenGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void OGLWidget::eulerChanged(double _t1, double _t2, double _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void OGLWidget::matrixChanged(const double * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void OGLWidget::translationChanged(const double * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void OGLWidget::toEulerClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void OGLWidget::resetClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void OGLWidget::applyClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void OGLWidget::captureClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void OGLWidget::saveClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void OGLWidget::loadClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void OGLWidget::autoCalibrateClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void OGLWidget::cropChanged(const float * _t1, const float * _t2, const float * _t3, const float * _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void OGLWidget::presetSelected(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void OGLWidget::saveCalibrationClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void OGLWidget::loadCalibrationClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
