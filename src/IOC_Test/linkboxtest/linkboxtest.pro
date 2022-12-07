TEMPLATE = app
#CONFIG += debug_and_release console
TARGET = linkboxtest

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

INCLUDEPATH += ../../libs/libIOCGpio
LIBS += -L../../libs/libIOCGpio -lIOCGpio -lgpiodcxx

INCLUDEPATH += ../../libs/libVSHAL
INCLUDEPATH += ../../libs/libVSHAL/BaseUnit/src-omap
INCLUDEPATH += ../../libs/libVSHAL/Common
LIBS += -L../../libs/libVSHAL -lVSHAL -L../../libs/libVSHAL/BaseUnit/src-omap -lVSHAL-BU

INCLUDEPATH += ../../libs/qextserialport/src
LIBS += -L../../libs/qextserialport/src/build -lqextserialport

INCLUDEPATH += ../../libs/libVSCommon
LIBS += -L../../libs/libVSCommon -lVSCommon

# Input
SOURCES += main.cpp \
	linkboxtester.cpp \
    uart.cpp \
#    iocontroller.cpp
#    synchronousiocontroller.cpp

HEADERS += \
	linkboxtester.h \
    uart.h \
#    iocontroller.h
#    synchronousiocontroller.h
