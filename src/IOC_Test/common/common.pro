TEMPLATE = lib
CONFIG += staticlib

QT += core
QT -= gui

DEPENDPATH += .
INCLUDEPATH += .
#include ( ../../../mkspecs/vitalsim2.pri )
include ( ../../prod.pri )
# qextserialport Library
INCLUDEPATH += ../../libs/qextserialport/src
include( ../../libs/qextserialport/qextserialport.pri )

# libVSHAL Library for Base Unit
#LIBVSHAL_BU = 1
#include( ../../libs/libVSHAL/libVSHAL.pri )

# libVSCommon Library
include( ../../libs/libVSCommon/libVSCommon.pri )


HEADERS = \
	SWversion.h \
	inputeventhandler.h \
	ioctrlcommController.h \
	itestcase.h \
	itestreporter.h \
	testmodewrapper.h

SOURCES = \
	inputeventhandler.cpp \
	ioctrlcommController.cpp \
	testmodewrapper.cpp
