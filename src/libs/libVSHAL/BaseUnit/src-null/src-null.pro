# The VitalSim2 setup
include ( ../../../../mkspecs/vitalsim2.pri )

TEMPLATE = lib
CONFIG -= qt
CONFIG += staticlib
TARGET = VSHAL-BU

# Set install pats
target.path = $$VS_LIB_PATH
INSTALLS += target

QMAKE_CXXFLAGS_RELEASE   += -std=c++0x
QMAKE_CXXFLAGE_DEBUG     += -std=c++0x

INCLUDEPATH += ../src-omap ../../Common

SOURCES += \
    HALLib.cpp \
    Version.cpp \
    ../src-omap/LED.cpp \
    ../src-omap/PWM.cpp \
    ../src-omap/Power.cpp \
    ../src-omap/IOController.cpp \
    ../src-omap/IO.cpp \
    ../src-omap/GPIO.cpp \
    ../../Common/HALLibBase.cpp \
    ../../Common/Lock.cpp \
    ../../Common/GPIOBase.cpp \
    ../../Common/ConfigParams.cpp

HEADERS += \
    ../src-omap/HALLib.h \
    ../src-omap/Version.h \
    ../src-omap/LED.h \
    ../src-omap/PWM.h \
    ../src-omap/Power.h \
    ../src-omap/IOController.h \
    ../src-omap/IO.h \
    ../src-omap/GPIO.h \
    ../src-omap/Types.h \
    ../../Common/HALLibBase.h \
    ../../Common/Lock.h \
    ../../Common/GPIOBase.h \
    ../../Common/ConfigParams.h \
    ../../Common/VersionBase.h \
    ../../Common/HALException.h
