# The VitalSim2 setup
#include ( ../../../../mkspecs/vitalsim2.pri )
include ( ../../../../prod.pri )
TEMPLATE = lib
CONFIG -= qt
TARGET = VSHAL-RC
QMAKE_CXXFLAGS += -std=c++0x
# Set install pats
target.path = $$VS_LIB_PATH
INSTALLS += target

INCLUDEPATH += ../../Common

SOURCES += \
    HALLib.cpp \
    Version.cpp \
    LCD.cpp \
    Power.cpp \
    GPIO.cpp \
    ../../Common/HALLibBase.cpp \
    ../../Common/Lock.cpp \
    ../../Common/GPIOBase.cpp \
    ../../Common/ConfigParams.cpp \
    ../../Common/crc32.cpp \
    ../../Common/File.cpp \
    Battery.cpp \
    LED.cpp

HEADERS += \
    HALLib.h \
    Version.h \
    LCD.h \
    Power.h \
    GPIO.h \
    Types.h \
    ../../Common/HALLibBase.h \
    ../../Common/Lock.h \
    ../../Common/GPIOBase.h \
    ../../Common/ConfigParams.h \
    ../../Common/VersionBase.h \
    ../../Common/HALException.h \
    ../../Common/crc32.h \
    ../../Common/File.h \
    Battery.h \
    LED.h
