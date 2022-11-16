include ( ../../prod.pri )
TEMPLATE = lib
CONFIG += staticlib

TARGET = IOCGpio
QT += \
    core \

QT -= gui

DEFINES += IOCGPIO_LIBRARY
INCLUDEPATH += ../../include
HEADERS += \
    gpio.h

LIBS += -lgpiodcxx

SOURCES += \
    gpio.cpp

OTHER_FILES =
