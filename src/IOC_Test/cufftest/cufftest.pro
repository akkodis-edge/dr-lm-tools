#-------------------------------------------------
#
# Project created by QtCreator 2011-09-01T09:35:49
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = cufftest
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

LIBVSHAL_BU = 1
INCLUDEPATH += ../../libs/libVSHAL
LIBS += -L../../libs/libVSHAL -lVSHAL

SOURCES += main.cpp \
    cufftester.cpp

HEADERS += \
    cufftester.h
