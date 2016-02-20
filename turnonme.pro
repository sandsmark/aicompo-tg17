QT += network quick

CONFIG += c++11

linux: QMAKE_CXXFLAGS += -DAPP_VERSION=\\\"`git rev-parse --short HEAD`\\\"

win32:RC_ICONS += turnon.ico

lessThan(QT_MAJOR_VERSION, 5): error("Requires Qt 5!")

# QMAKE_CXXFLAGS += -Wall -Werror -Wextra

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    player.cpp \
    gamemanager.cpp \
    networkclient.cpp \
    missile.cpp \
    settings.cpp

HEADERS += \
    player.h \
    gamemanager.h \
    networkclient.h \
    parameters.h \
    missile.h \
    settings.h

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    qml/Button.qml \
    qml/EndScreen.qml \
    qml/main.qml \
    qml/PlayerSprite.qml \
    qml/StartScreen.qml \
    qml/Checkbox.qml \
    qml/MissileSprite.qml \

DISTFILES += \
    sprites/missile-empty.png \
    sprites/missile-full.png \
    sprites/missile-half.png \
    sprites/turnon.png \
    sprites/sun.png
