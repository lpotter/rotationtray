HEADERS       = window.h
SOURCES       = main.cpp \
                window.cpp
RESOURCES     = rotationtray.qrc

QT += widgets sensors

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/desktop/rotationtray
INSTALLS += target
