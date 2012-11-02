TARGET = qios

load(qt_plugin)
DESTDIR = $$QT.gui.plugins/platforms

QT += opengl
QT += core-private gui-private platformsupport-private opengl-private widgets-private
LIBS += -framework UIKit

OBJECTIVE_SOURCES = main.mm \
    qiosintegration.mm \
    qioswindow.mm \
    qiosscreen.mm \
    qioseventdispatcher.mm \
    qiosbackingstore.mm

OBJECTIVE_HEADERS = qiosintegration.h \
    qioswindow.h \
    qiosscreen.h \
    qioseventdispatcher.h \
    qiosbackingstore.h

#HEADERS = qiossoftwareinputhandler.h

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
