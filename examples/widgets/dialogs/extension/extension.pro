HEADERS       = finddialog.h
SOURCES       = finddialog.cpp \
                main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/dialogs/extension
INSTALLS += target

QT += widgets
simulator: warning(This example might not fully work on Simulator platform)
