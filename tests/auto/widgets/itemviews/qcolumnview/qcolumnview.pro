CONFIG += testcase
CONFIG += parallel_test
QT += widgets widgets-private
QT += gui-private core-private testlib

SOURCES		+= tst_qcolumnview.cpp
TARGET		= tst_qcolumnview
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
