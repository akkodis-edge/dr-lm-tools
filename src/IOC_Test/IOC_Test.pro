TEMPLATE = subdirs
CONFIG  += ordered
include ( ../prod.pri )
SUBDIRS = \
	common \
	ioctest \
    pso \
    pulsedriver \
	gpiotopower \
	productiontest \
	linkboxtest \
	cufftest 

ioctest.depends = common
pso.depends = common
pulsedriver.depends = common
gpiotopower.depends = common
productiontest.depends = common
linkboxtest.depends = common
cufftest.depends = common
