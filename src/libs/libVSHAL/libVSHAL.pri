INCLUDEPATH += $$PWD/Common
!isEmpty(LIBVSHAL_BU):INCLUDEPATH += $$PWD/BaseUnit/src-omap
!isEmpty(LIBVSHAL_RC):INCLUDEPATH += $$PWD/RemoteControl/src-omap

QMAKE_CXXFLAGS += -std=c++0x
# Link against libVSHAL-BU for omap (hardcoded)
isEmpty(LIBVSHAL_BUILDDIR): LIBVSHAL_BUILDDIR=$$OUT_PWD
!contains(LIBVSHAL_BUILDDIR,$$OUT_PWD): LIBVSHAL_BUILDDIR=$$OUT_PWD/$$LIBVSHAL_BUILDDIR
# LIBS += -L$$LIBVSHAL_BUILDDIR/Common/$$DIR -lVSHAL-Common
!isEmpty(LIBVSHAL_BU) {
    LIBS += -L$$LIBVSHAL_BUILDDIR/BaseUnit/src-omap/$$DESTDIR -lVSHAL-BU
    isEmpty(VS_CROSS):LIBS += -Wl,-rpath,$$LIBVSHAL_BUILDDIR/BaseUnit/src-omap/$$DESTDIR
}
!isEmpty(LIBVSHAL_RC) {
    LIBS += -L$$LIBVSHAL_BUILDDIR/RemoteControl/src-omap/$$DESTDIR -lVSHAL-RC
    isEmpty(VS_CROSS):LIBS += -Wl,-rpath,$$LIBVSHAL_BUILDDIR/RemoteControl/src-omap/$$DESTDIR
}
