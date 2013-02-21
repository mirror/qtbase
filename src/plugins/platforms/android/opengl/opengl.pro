PLUGIN_TYPE = platforms
load(qt_plugin)

DEFINES += QT_STATICPLUGIN ANDROID_PLUGIN_OPENGL
CONFIG += dll

!contains(ANDROID_PLATFORM, android-9) {
    INCLUDEPATH += $$NDK_ROOT/platforms/android-9/arch-$$ANDROID_ARCHITECTURE/usr/include
    LIBS += -L$$NDK_ROOT/platforms/android-9/arch-$$ANDROID_ARCHITECTURE/usr/lib -ljnigraphics -landroid
} else : LIBS += -ljnigraphics -landroid

EGLFS_PLATFORM_HOOKS_SOURCES = $$PWD/../src/opengl/qeglfshooks_android.cpp

INCLUDEPATH += $$PWD/../src/opengl/
HEADERS += \
    $$PWD/../src/opengl/qandroidopenglcontext.h \
    $$PWD/../src/opengl/qandroidopenglplatformwindow.h

SOURCES += \
    $$PWD/../src/opengl/qandroidopenglcontext.cpp \
    $$PWD/../src/opengl/qandroidopenglplatformwindow.cpp


include($$PWD/../../eglfs/eglfs.pri)
include($$PWD/../src/src.pri)


TARGET = qtforandroidGL
target.path = $$[QT_INSTALL_PLUGINS]/platforms/android
INSTALLS += target

