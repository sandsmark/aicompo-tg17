import QtQuick 2.0

Rectangle {
    id: endScreen
    anchors.fill: parent
    enabled: visible
    visible: opacity > 0

    Behavior on opacity {
        NumberAnimation {
            duration: 500
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

    // List of players
    Rectangle {
        color: "#c0000000";
        id: userList
        anchors.top: gameOverText.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: quitButton.top
        anchors.margins: 20
        border.color: "white"
        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.margins: 20
            Repeater {
                model: endScreen.visible ? GameManager.players : 0 // Only fetch players when finished, to get them correctly sorted
                delegate: Text {
                    horizontalAlignment: Text.AlignHCenter
                    color: "white"
                    font.pointSize: 20
                    font.bold: true
                    font.family: "Aldrich"
                    text: (index + 1) + ". " + model.modelData.name + ": " + model.modelData.wins + " wins" + " (" + model.modelData.score + " hits)"
                }
            }
        }
    }

    // Quit button
    Button {
        id: quitButton
        anchors {
            bottom: parent.bottom
            bottomMargin: 10
            right: parent.horizontalCenter
            rightMargin: 10
        }

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
        visible: GameManager.players.length > 0

        onClicked: {
            if (GameManager.players.length < 1) {
                return
            }
            GameManager.stopGame()
        }
        text: "Restart"
    }
}
