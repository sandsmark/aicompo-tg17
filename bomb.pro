# Add more folders to ship with the application, here
folder_01.source = qml/bomb
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

QT += network quick multimedia

QMAKE_CXXFLAGS += -DAPP_VERSION=\\\"`git rev-parse --short HEAD`\\\"

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp tile.cpp \
    map.cpp \
    player.cpp \
    gamemanager.cpp \
    bomb.cpp \
    userplayer.cpp \
    networkclient.cpp

HEADERS += tile.h \
    map.h \
    player.h \
    gamemanager.h \
    bomb.h \
    userplayer.h \
    networkclient.h

RESOURCES += \
    sprites.qrc

OTHER_FILES += \
    default.map \
    Arena.map \
    qml/bomb/Button.qml \
    qml/bomb/MapSprite.qml \
    qml/bomb/BombSprite.qml \
    qml/bomb/EndScreen.qml \
    qml/bomb/main.qml \
    qml/bomb/PlayerSprite.qml \
    qml/bomb/PlayingField.qml \
    qml/bomb/StartScreen.qml \
    qml/bomb/Checkbox.qml
