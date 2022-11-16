# The VitalSim2 setup
#include ( ../../../mkspecs/vitalsim2.pri )

TEMPLATE = subdirs

SUBDIRS += omap # null
QMAKE_CXXFLAGS += -std=c++0x
omap.subdir = src-omap

null.subdir = src-null

testapp.depends = omap
