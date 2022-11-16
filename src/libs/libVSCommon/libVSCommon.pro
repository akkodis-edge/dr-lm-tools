# The VitalSim2 setup

include ( ../../prod.pri )
TEMPLATE = lib
CONFIG += staticlib

TARGET = VSCommon
QT += \
    core \
    xml \
    network

QT -= gui

DEFINES += VSCOMMON_LIBRARY
INCLUDEPATH += ../../include
HEADERS += \
    Communication/CrcCCITT.h \
    Communication/CommunicationIDs.h 

SOURCES += \
    Communication/CrcCCITT.cpp

OTHER_FILES =
