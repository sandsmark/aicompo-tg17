# Add more folders to ship with the application, here
folder_01.source = qml/bomb
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp tile.cpp \
    map.cpp \
    player.cpp \
    gamemanager.cpp \
    bomb.cpp \
    userplayer.cpp

HEADERS += tile.h \
    map.h \
    player.h \
    gamemanager.h \
    bomb.h \
    userplayer.h

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

RESOURCES += \
    sprites.qrc

OTHER_FILES += \
    default.map \
    qml/bomb/MapSprite.qml
