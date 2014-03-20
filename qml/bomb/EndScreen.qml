import QtQuick 2.0

Rectangle {
    id: endScreen
    anchors.fill: parent
    enabled: opacity > 0.1 ? true : false

    Behavior on opacity {
        NumberAnimation {
            easing { type: Easing.OutElastic; amplitude: 1.0; period: 0.9 }
            duration: 5000
        }
    }

    color: "#a0000000"
    Text {
        id: gameOverText
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 20
        font.pointSize: 25
        font.bold: true
        text: "Game Over"
        color: "white"
        style: Text.Outline
    }

    // List of connected users
    Rectangle {
        color: "#c0000000";
        id: userList
        anchors.top: gameOverText.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: quitButton.top
        anchors.margins: 20
        width: 600
        border.color: "white"
        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.margins: 20
            Repeater {
                model: players
                delegate: Text {
                    horizontalAlignment: Text.AlignHCenter
                    color: model.modelData.alive ? "green" : "red"
                    font.pixelSize: 45
                    font.bold: true
                    text: model.modelData.name + ": " + model.modelData.wins + " wins"
                }
            }
        }
    }

    // Quit button
    Button {
        id: quitButton
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.right: parent.horizontalCenter
        anchors.rightMargin: 10
        width: 200
        height: 50

        onClicked: {
            Qt.quit()
        }
        text: "Quit"
    }

    // Restart button
    Button {
        id: restartButton
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.left: parent.horizontalCenter
        anchors.leftMargin: 10
        width: 200
        height: 50
        visible: players.length > 0

        onClicked: {
            if (players.length < 1) {
                return
            }
            endScreen.opacity = 0
            startScreen.opacity = 1
            //game.restartGame()
        }
        text: "Restart"
    }
}
