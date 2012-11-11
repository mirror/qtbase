TARGET = qiosmain

load(qt_plugin)
DESTDIR = $$QT.gui.plugins/platforms

OBJECTIVE_SOURCES = qtmain.mm

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
