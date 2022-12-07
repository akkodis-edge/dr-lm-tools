TEMPLATE = app
#CONFIG += debug_and_release console
TARGET = productiontest

QT += core testlib
QT -= gui

# VitalSim2
include ( ../../prod.pri )
# install paths
target.path = $$VS_BIN_PATH
INSTALLS += target

extra.path = $$VS_CONF_PATH
INSTALLS += extra

INCLUDEPATH += ../../include

INCLUDEPATH += ../common
LIBS += -L../common -lcommon

INCLUDEPATH += ../../libs/qextserialport/src
LIBS += -L../../libs/qextserialport/src/build -lqextserialport

INCLUDEPATH += ../../libs/libVSCommon
LIBS += -L../../libs/libVSCommon -lVSCommon

#LIBVSHAL_BU = 1
#include( ../../libs/libVSHAL/libVSHAL.pri )
INCLUDEPATH += ../../libs/libVSHAL
INCLUDEPATH += ../../libs/libVSHAL/BaseUnit/src-omap
INCLUDEPATH += ../../libs/libVSHAL/Common
LIBS += -L../../libs/libVSHAL -lVSHAL -L../../libs/libVSHAL/BaseUnit/src-omap -lVSHAL-BU

# libgpio
#INCLUDEPATH += /usr/lib/aarch64-linux-gnu/lgpiod/include/
#LIBS += -L "/usr/lib/aarch64-linux-gnu/lgpiod/lib" -lgpiodcxx -L "/usr/lib/aarch64-linux-gnu/lgpiod/lib" -lgpiod

# Input
SOURCES += productiontest.cpp 
