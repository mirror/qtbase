TARGET = qeglfs

PLUGIN_TYPE = platforms
load(qt_plugin)

SOURCES += $$PWD/main.cpp

include(eglfs.pri)
