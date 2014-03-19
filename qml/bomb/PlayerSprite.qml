import QtQuick 2.0

Image {
    property int playerId
    id: image
    smooth: false
    property int walkCycle: 0
    property string direction: "s"
    property string command: lastCommand

    source: alive ?
                "qrc:/sprites/character" + ((playerId % 2)+1)  + "/character-" + direction + "-" + walkCycle + ".png" :
                "qrc:/sprites/blood.png"


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
        enabled: alive
        NumberAnimation {
            id: xAnimation
            easing { type: Easing.OutQuad; amplitude: 1.0; period: 0.9 }
            duration: 150
            onRunningChanged: {
                if (running) {
                    animTimer.start()
                    yAnimation.complete()
                } else {
                    animTimer.stop()
                    walkCycle = 0
                }
            }
        }
    }

    Behavior on y {
        NumberAnimation {
            id: yAnimation
            easing { type: Easing.OutQuad; amplitude: 1.0; period: 0.9 }
            duration: 150
            onRunningChanged: {
                if (running) {
                    animTimer.start()
                    xAnimation.complete()
                } else {
                    animTimer.stop()
                    walkCycle = 0
                }
            }
        }
    }


    Timer {
        property int count: 0
        id: animTimer
        interval: 50
        repeat: true
        running: false
        onTriggered: {
            if (walkCycle >= 3) {
                walkCycle = 0
            } else {
                walkCycle += 1
            }
        }
    }

    Text {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        color: "white"
        text: name
        font.bold: true
        style: Text.Outline
    }


    Rectangle {
        id: messagebox
        anchors.top: parent.bottom
        anchors.topMargin: -30
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#af000000"
        radius: 5
        width: 100
        height: 50
        opacity: 0
        NumberAnimation on opacity {
            id: messageboxFade
            running: false
            easing.type: "InQuint"
            from: 0.75
            to: 0
            duration: 5000
        }

        Text {
            anchors.fill: parent
            anchors.margins: 5
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            color: "white"
            text: message
            font.bold: true
            onTextChanged: if(text !== "") messageboxFade.restart()
            elide: Text.ElideRight
            wrapMode: Text.Wrap
        }
    }
}
