TARGET = qevdevkeyboardplugin

PLUGIN_TYPE = generic
load(qt_plugin)

QT += core-private platformsupport-private gui-private

SOURCES = main.cpp

OTHER_FILES += \
    evdevkeyboard.json

