QT += network quick

CONFIG += c++11 optimize_full

linux {
    QMAKE_CXXFLAGS += -DAPP_VERSION=\\\"`git rev-parse --short HEAD`\\\"
    QT += widgets
}

win32:RC_ICONS += ghostly.ico

lessThan(QT_MAJOR_VERSION, 5): error("Requires Qt 5!")

# QMAKE_CXXFLAGS += -Wall -Werror -Wextra

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    player.cpp \
    gamemanager.cpp \
    networkclient.cpp \
    settings.cpp \
    map.cpp \
    monster.cpp

HEADERS += \
    player.h \
    gamemanager.h \
    networkclient.h \
    parameters.h \
    settings.h \
    map.h \
    monster.h

RESOURCES += \
    resources.qrc
