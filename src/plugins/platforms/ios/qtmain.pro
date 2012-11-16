TARGET = qiosmain

PLUGIN_TYPE = platforms
load(qt_plugin)
DESTDIR = $$QT.gui.plugins/platforms

OBJECTIVE_SOURCES = qtmain.mm \
    qiosviewcontroller.mm

HEADERS = qiosviewcontroller.h
