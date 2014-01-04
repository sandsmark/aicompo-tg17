import QtQuick 2.0

Image {
    property int playerId
    id: image
    smooth: false
    height: 64
    width: 64
    property bool isAlive: alive
    source: "qrc:/sprites/character" + (playerId+1)  + "/character-s-0.png"
    onIsAliveChanged: {
        if (!isAlive) {
            source = "qrc:/sprites/blood.png"
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


