HEADERS     = mainwindow.h
SOURCES     = main.cpp \
              mainwindow.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/richtext/calendar
INSTALLS += target

QT += widgets
