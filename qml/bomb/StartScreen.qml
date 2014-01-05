import QtQuick 2.0

Rectangle {
    id: startScreen
    width: 640
    height: 480
    anchors.centerIn: parent
    color: "#cc000000"
    border.color: "white"

    Rectangle {
        id: humanPlayerCheckbox
        height: 20
        width: 20
        anchors.bottom: userList.top
        anchors.bottomMargin: 30
        anchors.right: parent.right
        anchors.rightMargin: 30
        border.color: "white"
        color: "transparent"

        Rectangle {
            id: checkMark
            anchors.fill: parent
            anchors.margins: 2
            opacity: 0
            property bool checked: false
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: MouseArea.PointingHandCursor
            hoverEnabled: true
            onEntered: {
                if (checkMark.checked) {
                    checkMark.opacity = 0.7
                } else {
                    checkMark.opacity = 0.3
                }
            }
            onExited: {
                if (checkMark.checked) {
                    checkMark.opacity = 1.0
                } else {
                    checkMark.opacity = 0.0
                }
            }


            onClicked: {
                if (checkMark.checked) {
                    game.removeHumanPlayers()
                    if (containsMouse) {
                        checkMark.opacity = 0.3
                    } else {
                        checkMark.opacity = 0.0
                    }
                    checkMark.checked = false
                } else {
                    game.addPlayer()
                    if (containsMouse) {
                        checkMark.opacity = 0.7
                    } else {
                        checkMark.opacity = 1.0
                    }
                    checkMark.checked = true
                }
            }
        }
    }

    Text {
        anchors.right: humanPlayerCheckbox.left
        anchors.verticalCenter: humanPlayerCheckbox.verticalCenter
        anchors.rightMargin: 10
        text: "Enable human player:"
        color: "white"
    }

    Text {
        id: topLabel
        font.pixelSize: 20
        color: "red"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.topMargin: 30
        text: "Connected users:"
    }

    // List of connected users
    Column {
        id: userList
        anchors.top: topLabel.bottom
        anchors.left: parent.left
        anchors.topMargin: 20
        anchors.leftMargin: 20
        Repeater {
            model: players
            delegate: Text {
                color: "white"
                font.pixelSize: 15
                text: model.modelData.name
            }
        }
    }

    // Start button
    Rectangle {
        id: startButton
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        width: 200
        height: 50

        color: "#50000000"
        border.color: "white"
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: MouseArea.PointingHandCursor
            onEntered: {
                startButtonText.color = "black"
                startButton.color = "white"
            }
            onExited: {
                startButtonText.color = "white"
                startButton.color = "#50000000"
            }
            onClicked: {
                if (players.length < 2) {
                    return
                }
                startScreen.visible = false
                game.gameStart()
            }
        }
        Text {
            id: startButtonText
            anchors.centerIn: parent
            text: "Start game"
            color: "white"
            font.pointSize: 25
        }
    }
}

