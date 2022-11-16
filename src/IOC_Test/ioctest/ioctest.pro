TEMPLATE = app
CONFIG += console
TARGET = ioctest


QT += core
QT -= gui

include ( ../../prod.pri )
INCLUDEPATH += ../../include

INCLUDEPATH += ../common
LIBS += -L../common -lcommon

INCLUDEPATH += ../../libs/qextserialport/src
LIBS += -L../../libs/qextserialport/src/build -lqextserialport

INCLUDEPATH += ../../libs/libVSCommon
LIBS += -L../../libs/libVSCommon -lVSCommon


# Input
HEADERS += \
	pollanalog.h \
	pollgpio.h \
	pollpso.h \
	safeguardpower.h \
	testanalog.h \
	testcuff.h \
	testgpio.h \
	testgpo.h \
	testpower.h \
	testpso.h \
	testpulsedriver.h \
	testrunner.h \
	textreporter.h \

SOURCES += \
	main.cpp \
	pollanalog.cpp \
	pollgpio.cpp \
	pollpso.cpp \
	safeguardpower.cpp \
	testanalog.cpp \
	testcuff.cpp \
	testgpio.cpp \
	testgpo.cpp \
	testpower.cpp \
	testpso.cpp \
	testpulsedriver.cpp \
	testrunner.cpp \
	textreporter.cpp
