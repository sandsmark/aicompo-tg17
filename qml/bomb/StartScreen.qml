import QtQuick 2.0

Rectangle {
    id: startScreen
    width: 800
    height: 600
    anchors.centerIn: parent
    color: "#cc000000"
    border.color: "white"

    Rectangle {
        id: humanPlayerCheckbox
        height: 20
        width: 20
        anchors.top: address.bottom
        anchors.topMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 30
        border.color: "white"
        color: "transparent"
        opacity: (players.length < map.maxPlayers || checkMark.checked) ? 1 : 0
        enabled: players.length < map.maxPlayers || checkMark.checked

        Behavior on opacity {
            NumberAnimation {
                duration: 250
            }
        }

        Rectangle {
            id: checkMark
            anchors.fill: parent
            anchors.margins: 3
            property bool checked: false
            border.color: "white"
            opacity: 0
            border.width: 1
            color: checkMark.checked ? "white" : "black"
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: MouseArea.PointingHandCursor
            hoverEnabled: true
            onEntered: {
                if (checkMark.checked) {
                    checkMark.opacity = 1.0
                } else {
                    checkMark.opacity = 0.3
                }
            }
            onExited: {
                if (checkMark.checked) {
                    checkMark.opacity = 0.7
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
                    if (players.length > map.maxPlayers) return;
                    game.addPlayer()
                    if (containsMouse) {
                        checkMark.opacity = 1.0
                    } else {
                        checkMark.opacity = 0.7
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
        font.pixelSize: 20
        color: "white"
        opacity: humanPlayerCheckbox.opacity
    }
/*
    Rectangle {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 30
        anchors.rightMargin: 30
        height: 30
        width: 350
        border.color: "white"
        color: "#c0000000";
        TextInput {
            id: ipInput
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 200
            anchors.margins: 2
            verticalAlignment: TextInput.AlignVCenter
            font.pixelSize: 20
            color:"white"
            inputMask: "000.000.000.000;_"
            opacity: focus ? 1 : 0.5
            onFocusChanged: if (focus) cursorPosition = 0
        }
        Text {
            verticalAlignment: TextInput.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            anchors.fill: ipInput
            font.pixelSize: 20
            text: "remote address"
            opacity: ipInput.focus ? 0 : 0.9
            color: "white"
        }
        Button {
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 150
            text: "connect"
        }
    }

*/
    Text {
        id: address
        color: "white"
        font.pixelSize: 20
        text: "Listening on: " + game.address
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.topMargin: 30
    }

    Text {
        id: topLabel
        font.pixelSize: 20
        color: "white"
        anchors.top: address.bottom
        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.topMargin: 10
        text: "Connected users:"
    }

    Text {
        anchors.left: topLabel.right
        anchors.leftMargin: 10
        anchors.bottom: topLabel.bottom
        color: players.length < 2 ? "red" : "green"
        text: players.length + "/" + map.maxPlayers;
        font.pixelSize: 20
        font.bold: true
        Behavior on color {
            ColorAnimation { duration: 200 }
        }
    }

    // List of connected users
    Rectangle {
        color: "#c0000000";
        id: userList
        anchors.top: topLabel.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: startButton.top
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
                    text: model.modelData.name
                }
            }
        }
    }

    // Start button
    Button {
        id: startButton
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.right: parent.horizontalCenter
        anchors.rightMargin: 10
        width: 200
        height: 50
        text: "Start game"
        onClicked: {
            if (players.length < 1) {
                return
            }
            startScreen.visible = false
            game.startRound()
        }
    }

    Rectangle {
        id: mapList
        width: mapSelect.width
        height: visible ? mapsRepeater.model.length * 50 : 0
        anchors.bottom: mapSelect.top
        anchors.right: mapSelect.right
        visible: false
        color: "#c0000000"
        border.color: "white"

        Behavior on height {
            NumberAnimation {
                easing { type: Easing.OutElastic; amplitude: 1.0; period: 0.9 }
                duration: 500
            }
        }

        Column {
            id: maplist
            anchors.fill: parent
            Repeater {
                id: mapsRepeater
                model: ["Arena", "default"]
                delegate: Rectangle {
                    id: mapSelection
                    width: 200
                    height: mapList.height/mapsRepeater.model.length

                    color: "transparent"
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: mapList.visible
                        cursorShape: MouseArea.PointingHandCursor
                        onEntered: {
                            mapSelectionText.color = "black"
                            mapSelection.color = "white"
                        }
                        onExited: {
                            mapSelectionText.color = "white"
                            mapSelection.color = "#50000000"
                        }
                        onClicked: {
                            mapList.visible = false
                            game.loadMap(modelData)
                        }
                    }
                    Text {
                        id: mapSelectionText
                        anchors.fill: parent
                        anchors.leftMargin: 10
                        text: "- " + modelData
                        color: "white"
                        font.pointSize: 25
                    }
                }
            }
        }
    }

    Rectangle {
        id: mapSelect
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
                mapSelectText.color = "black"
                mapSelect.color = "white"
            }
            onExited: {
                mapSelectText.color = "white"
                mapSelect.color = "#50000000"
            }
            onClicked: {
                mapList.visible = !mapList.visible
            }
        }
        Text {
            id: mapSelectText
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: mapSelect.verticalCenter
            text: map.name
            color: "white"
            font.pointSize: 25
        }
        Rectangle {
            anchors.right: mapSelect.right
            anchors.rightMargin: 10
            anchors.verticalCenter: mapSelect.verticalCenter
            color: "transparent"
            border.color: mapSelectText.color
            border.width: 3
            height: 40
            width: 40
            Text {
                anchors.centerIn: parent
                font.pointSize: 25
                color: mapSelectText.color
                text: mapList.visible ? "[]" : "="
            }
        }
    }
}

