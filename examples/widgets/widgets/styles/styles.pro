HEADERS       = norwegianwoodstyle.h \
                widgetgallery.h
SOURCES       = main.cpp \
                norwegianwoodstyle.cpp \
                widgetgallery.cpp
RESOURCES     = styles.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/styles
INSTALLS += target

QT += widgets

simulator: warning(This example might not fully work on Simulator platform)
