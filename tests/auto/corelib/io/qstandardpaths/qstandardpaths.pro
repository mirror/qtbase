load(qttest_p4)
SOURCES += tst_qstandardpaths.cpp
QT = core
CONFIG += parallel_test

wince* {
    DEFINES += SRCDIR=\\\"\\\"
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

