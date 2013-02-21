PLUGIN_TYPE = platforms

TARGET = qtforandroid
load(qt_plugin)

DEFINES += QT_STATICPLUGIN

CONFIG += dll

!contains(ANDROID_PLATFORM, android-9) {
    INCLUDEPATH += $$NDK_ROOT/platforms/android-9/arch-$$ANDROID_ARCHITECTURE/usr/include
    LIBS += -L$$NDK_ROOT/platforms/android-9/arch-$$ANDROID_ARCHITECTURE/usr/lib -ljnigraphics -landroid
} else : LIBS += -ljnigraphics -landroid

include($$PWD/../src/src.pri)
include($$PWD/../src/raster/raster.pri)

INSTALLS += target
