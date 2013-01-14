CONFIG += testcase
TARGET = tst_qaccessibility
requires(contains(QT_CONFIG,accessibility))
QT += widgets testlib gui-private
SOURCES  += tst_qaccessibility.cpp

unix:!mac:LIBS+=-lm

wince*: {
	accessneeded.files = $$QT_BUILD_TREE\\plugins\\accessible\\*.dll
	accessneeded.path = accessible
	DEPLOYMENT += accessneeded
}

win32 {
    !*g++ {
        include(../../../../src/3rdparty/iaccessible2/iaccessible2.pri)
        DEFINES += QT_SUPPORTS_IACCESSIBLE2
    }
    LIBS += -loleacc -loleaut32 -lole32 -luuid
}
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
