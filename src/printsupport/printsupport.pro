TARGET     = QtPrintSupport
QT = core-private gui-private widgets-private

DEFINES   += QT_NO_USING_NAMESPACE

android {
    DEFINES += QT_NO_PRINTDIALOG
}

QMAKE_DOCS = $$PWD/doc/qtprintsupport.qdocconf

load(qt_module)

QMAKE_LIBS += $$QMAKE_LIBS_PRINTSUPPORT

include(kernel/kernel.pri)
include(widgets/widgets.pri)
include(dialogs/dialogs.pri)
