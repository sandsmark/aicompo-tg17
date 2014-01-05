import QtQuick 2.0

Image {
    property int playerId
    id: image
    smooth: false
    height: 64
    width: 64
    property bool isAlive: alive
    property int walkCycle: 0
    property string direction: "s"
    property string command: lastCommand


    source: "qrc:/sprites/character" + (playerId+1)  + "/character-" + direction + "-" + walkCycle + ".png"
    onIsAliveChanged: {
        if (!isAlive) {
            source = "qrc:/sprites/blood.png"
        }
    }

    onCommandChanged: {
        if (command === "UP") {
            direction = "n"
        } else if (command === "DOWN") {
            direction = "s"
        } else if (command === "RIGHT") {
            direction = "e"
        } else if (command === "LEFT") {
            direction = "w"
        }
    }

    Behavior on x {
        NumberAnimation {
            easing { type: Easing.OutElastic; amplitude: 1.0; period: 0.9 }
            duration: 200
        }
    }

    Behavior on y {
        NumberAnimation {
            easing { type: Easing.OutElastic; amplitude: 1.0; period: 0.9 }
            duration: 200
        }
    }

}


