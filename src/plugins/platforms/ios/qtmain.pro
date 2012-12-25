TARGET = qiosmain
MODULE = iosmain

PLUGIN_TYPE = platforms
load(qt_plugin)
DESTDIR = $$QT.gui.plugins/platforms
QT += gui-private

OBJECTIVE_SOURCES = qtmain.mm
