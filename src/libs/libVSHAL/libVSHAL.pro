# The VitalSim2 setup
#include ( ../../mkspecs/vitalsim2.pri )
include ( ../../prod.pri )
TEMPLATE = lib
CONFIG = staticlib

TARGET = VSHAL

SUBDIRS += BaseUnit RemoteControl

BaseUnit.subdir = BaseUnit
RemoteControl.subdir = RemoteControl
Common.subdir = Common
