# The VitalSim2 setup

TEMPLATE = lib
CONFIG -= qt
CONFIG += staticlib
TARGET = VSHAL-BU
QMAKE_CXXFLAGS += -std=c++0x
# Set install paths
include ( ../../../../prod.pri )
target.path = $$VS_LIB_PATH
INSTALLS += target

INCLUDEPATH += ../../Common

SOURCES += \
    HALLib.cpp \
    Version.cpp \
    LED.cpp \
    PWM.cpp \
    Power.cpp \
    IOController.cpp \
    IO.cpp \
    GPIO.cpp \
    ../../Common/HALLibBase.cpp \
    ../../Common/Lock.cpp \
    ../../Common/GPIOBase.cpp \
    ../../Common/ConfigParams.cpp \
    ../../Common/crc32.cpp \
    Battery.cpp \
    ../../Common/File.cpp

HEADERS += \
    HALLib.h \
    Version.h \
    LED.h \
    PWM.h \
    Power.h \
    IOController.h \
    IO.h \
    GPIO.h \
    Types.h \
    ../../Common/HALLibBase.h \
    ../../Common/Lock.h \
    ../../Common/GPIOBase.h \
    ../../Common/ConfigParams.h \
    ../../Common/VersionBase.h \
    ../../Common/HALException.h \
    ../../Common/crc32.h \
    Battery.h \
    ../../Common/File.h
