TEMPLATE = app
TARGET = pulsedriver
DEPENDPATH += .
INCLUDEPATH += .

QT -= gui
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

INCLUDEPATH += ../../libs/libVSHAL
LIBS += -L../../libs/libVSHAL -lVSHAL

# Input
HEADERS += \
        pulsedriver.hpp

SOURCES += \
        main.cpp \
        pulsedriver.cpp

