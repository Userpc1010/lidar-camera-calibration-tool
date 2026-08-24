
QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CalibrationCore
TEMPLATE = app

LIBS += -L$$DESTDIR/ -lrealsense2

QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS += -fopenmp

LIBS += -fopenmp

CONFIG += c++17

SOURCES += main.cpp \
    icp.cpp \
    mainwindow.cpp \
    registration/registration.cpp \
    registration/registration_helper.cpp \
    rgbd_camera.cpp

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    CalibrationParams.h \
    LivoxData.hpp \
    ann/flat_container.hpp \
    ann/gaussian_voxelmap.hpp \
    ann/incremental_voxelmap.hpp \
    ann/kdtree.hpp \
    ann/kdtree_omp.hpp \
    ann/kdtree_tbb.hpp \
    ann/knn_result.hpp \
    ann/projection.hpp \
    ann/projective_search.hpp \
    ann/sequential_voxelmap_accessor.hpp \
    ann/traits.hpp \
    factors/general_factor.hpp \
    factors/gicp_factor.hpp \
    factors/icp_factor.hpp \
    factors/plane_icp_factor.hpp \
    factors/robust_kernel.hpp \
    icp.h \
    mainwindow.h \
    nanoflann.hpp \
    points/eigen.hpp \
    points/point_cloud.hpp \
    points/traits.hpp \
    registration/optimizer.hpp \
    registration/reduction.hpp \
    registration/reduction_omp.hpp \
    registration/reduction_tbb.hpp \
    registration/registration.hpp \
    registration/registration_helper.hpp \
    registration/registration_result.hpp \
    registration/rejector.hpp \
    registration/termination_criteria.hpp \
    rgbd_camera.h \
    util/downsampling.hpp \
    util/downsampling_omp.hpp \
    util/downsampling_tbb.hpp \
    util/fast_floor.hpp \
    util/lie.hpp \
    util/normal_estimation.hpp \
    util/normal_estimation_omp.hpp \
    util/normal_estimation_tbb.hpp \
    util/sort_omp.hpp \
    util/sort_tbb.hpp \
    util/vector3i_hash.hpp

INCLUDEPATH += $$PWD/../CalibrationCore\
INCLUDEPATH += /usr/include/librealsense2
DEPENDPATH += $$PWD/../CalibrationCore

unix:!macx: LIBS += -L$$PWD/../../../../usr/local/lib/ -lrealsense2-gl

INCLUDEPATH += $$PWD/../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../usr/local/include

FORMS +=

unix:!macx: LIBS += -L$$OUT_PWD/../CalibrationGUI/ -lCalibrationGUI

LIBS += -llivox_lidar_sdk_shared

INCLUDEPATH += $$PWD/../CalibrationGUI
DEPENDPATH += $$PWD/../CalibrationGUI

unix:!macx: LIBS += -L$$PWD/../../../usr/local/lib/ -lglfw

INCLUDEPATH += $$PWD/../../../usr/local/include
DEPENDPATH += $$PWD/../../../usr/local/include

INCLUDEPATH += /usr/local/include/opencv4
DEPENDPATH += /usr/local/include/opencv4

#set package support if disabled
QT_CONFIG -= no-pkg-config

#link opencv4 package
CONFIG += link_pkgconfig
PKGCONFIG += opencv4

CONFIG += link_pkgconfig
PKGCONFIG += eigen3

