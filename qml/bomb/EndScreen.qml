import QtQuick 2.0

Rectangle {
    id: endScreen
    anchors.fill: parent
    width: 100
    height: 62

    Behavior on opacity {
        NumberAnimation {
            easing { type: Easing.OutElastic; amplitude: 1.0; period: 0.9 }
            duration: 5000
        }
    }

    color: "#a0000000"
    Text {
        anchors.centerIn: parent
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
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: quitButton.top
        anchors.margins: 20
        border.color: "white"
        Column {
            anchors.fill: parent
            anchors.margins: 20
            Repeater {
                model: players
                delegate: Text {
                    color: "white"
                    font.pixelSize: 15
                    text: model.modelData.name + ": " + (model.modelData.alive ? "Alive" : "Dead")
                }
            }
        }
    }

    // Start button
    Rectangle {
        id: quitButton
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.right: parent.horizontalCenter
        anchors.rightMargin: 10
        width: 200
        height: 50

        color: "#50000000"
        border.color: "white"
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: MouseArea.PointingHandCursor
            onEntered: {
                quitButtonText.color = "black"
                quitButton.color = "white"
            }
            onExited: {
                quitButtonText.color = "white"
                quitButton.color = "#50000000"
            }
            onClicked: {
                if (players.length < 2) {
                    return
                }
                Qt.quit()
            }
        }
        Text {
            id: quitButtonText
            anchors.centerIn: parent
            text: "Quit"
            color: "white"
            font.pointSize: 25
        }
    }

    // Start button
    Rectangle {
        id: restartButton
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.left: parent.horizontalCenter
        anchors.leftMargin: 10
        width: 200
        height: 50

        color: "#50000000"
        border.color: "white"
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: MouseArea.PointingHandCursor
            onEntered: {
                restartButtonText.color = "black"
                restartButton.color = "white"
            }
            onExited: {
                restartButtonText.color = "white"
                restartButton.color = "#50000000"
            }
            onClicked: {
                if (players.length < 2) {
                    return
                }
                endScreen.opacity = 0
                game.gameRestart()
            }
        }
        Text {
            id: restartButtonText
            anchors.centerIn: parent
            text: "Restart"
            color: "white"
            font.pointSize: 25
        }
    }
}
