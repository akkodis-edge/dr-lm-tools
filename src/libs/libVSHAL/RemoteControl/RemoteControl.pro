# The VitalSim2 setup
#include ( ../../../mkspecs/vitalsim2.pri )
include ( ../../../prod.pri )
TEMPLATE = subdirs

SUBDIRS += omap # null
isEmpty(VS_RELEASE):SUBDIRS += testapp

omap.subdir = src-omap

null.subdir = src-null

testapp.subdir = test-app
testapp.depends = omap
