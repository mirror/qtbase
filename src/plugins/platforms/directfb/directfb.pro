TARGET = qdirectfb
load(qt_plugin)
DESTDIR = $$QT.gui.plugins/platforms

QT += core-private gui-private platformsupport-private

isEmpty(DIRECTFB_LIBS) {
    DIRECTFB_LIBS = -ldirectfb -lfusion -ldirect -lpthread
}
isEmpty(DIRECTFB_INCLUDEPATH) {
    DIRECTFB_INCLUDEPATH = /usr/include/directfb
}

INCLUDEPATH += $$DIRECTFB_INCLUDEPATH
LIBS += $$DIRECTFB_LIBS

SOURCES = main.cpp \
    qdirectfbintegration.cpp \
    qdirectfbbackingstore.cpp \
    qdirectfbblitter.cpp \
    qdirectfbconvenience.cpp \
    qdirectfbinput.cpp \
    qdirectfbcursor.cpp \
    qdirectfbwindow.cpp
HEADERS = qdirectfbintegration.h \
    qdirectfbbackingstore.h \
    qdirectfbblitter.h \
    qdirectfbconvenience.h \
    qdirectfbinput.h \
    qdirectfbcursor.h \
    qdirectfbwindow.h

# ### port the GL context

CONFIG += qpa/genericunixfontdatabase
target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
