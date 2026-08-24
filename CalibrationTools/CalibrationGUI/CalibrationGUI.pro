QT += core gui
QT += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): CONFIG += c++17
lessThan(QT_MAJOR_VERSION, 5): QMAKE_CXXFLAGS += -std=c++17

TARGET = CalibrationGUI
TEMPLATE = lib

QMAKE_CXXFLAGS += -O3
QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS += -fopenmp

LIBS += -fopenmp

include(qtimgui/qtimgui.pri)

SOURCES += \
        oglwidget.cpp \
        simpleobject3d.cpp \
    camera_3d.cpp \
    cube.cpp \
    cuberander.cpp \
    drawline.cpp \
    multidrawline.cpp

HEADERS += \
        oglwidget.h \
        simpleobject3d.h \
    camera_3d.h \
    cube.h \
    cuberander.h \
    drawline.h \
    multidrawline.h

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Recurce/shaders.qrc \
    Recurce/textures.qrc \
    Recurce/model.qrc

