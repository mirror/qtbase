SOURCES = main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/statemachine/eventtransitions
INSTALLS += target
QT += widgets


simulator: warning(This example might not fully work on Simulator platform)
