# The VitalSim2 setup
#include ( ../../../../mkspecs/vitalsim2.pri )
include ( ../../../../prod.pri )
TEMPLATE = app
CONFIG -= qt
TARGET = hal-test-rc
QMAKE_CXXFLAGS += -std=c++0x
# Set install pats
target.path = $$VS_BIN_PATH
INSTALLS += target

# qextserialport Library
LIBVSHAL_BUILDDIR = ../..
LIBVSHAL_RC = 1
include( ../../libVSHAL.pri )

SOURCES += \
    hallibtest.cpp \
    ../../Common/hallibtest_common.cpp \
    ../../Common/getconfigpath-rc.cpp

HEADERS += \
    hallibtest.h \
    ../../Common/hallibtest_common.h \
    ../../Common/getconfigpath.h
