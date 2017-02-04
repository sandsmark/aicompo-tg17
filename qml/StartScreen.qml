import QtQuick 2.0
import QtQuick.Particles 2.0
import QtGraphicalEffects 1.0
import org.gathering.ghostly 1.0

Item {
    id: startScreen
    anchors.fill: parent

    visible: opacity > 0

    Behavior on opacity {
        NumberAnimation {
            duration: 500
        }
    }

    Emitter {
        anchors.top: parent.top
        anchors.bottom: titleText.top
        anchors.bottomMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        width: height
        enabled: parent.opacity > 0.5
        emitRate: 1000
        lifeSpan: 500
        size: 15
//        sizeVariation: 5

        shape: MaskShape {
            source: "qrc:///sprites/logo.png"
        }
//        shape: EllipseShape { }

//        velocity: PointDirection {
//            xVariation: 10
//            yVariation: 10
//        }
                velocity: AngleDirection{ magnitude: 2; magnitudeVariation: 1; angleVariation: 360}

        system: particleSystem
        group: "Logo"
    }

    Text {
        id: titleText
        anchors {
            bottom: settingsList.top
            bottomMargin: 20
            horizontalCenter: parent.horizontalCenter
        }
        verticalAlignment: Text.AlignVCenter
        text: "ghostly"
        color: "white"
        font.pixelSize: 160
        antialiasing: false
        renderType: Text.NativeRendering
    }

    Rectangle {
        id: settingsList
        height: 100

        anchors {
            bottom: playerList.top
            right: playerList.right
            left: playerList.horizontalCenter
            leftMargin: 5
            bottomMargin: 10
        }

        border.width: 4
        border.color: "white"
        color: "#7f000000"

        Checkbox {
            id: humanPlayerCheckbox
            anchors.top: parent.top
            anchors.topMargin: 15
            anchors.right: parent.right
            anchors.rightMargin: 10
            opacity: (GameManager.players.length < GameManager.maxPlayers || checked) ? 1 : 0
            enabled: GameManager.players.length < GameManager.maxPlayers || checked
            onClicked: {
                if (checked) {
                    GameManager.removeHumanPlayer()
                    checked = false
                } else {
                    if (GameManager.players.length >= GameManager.maxPlayers) return;

                    GameManager.addPlayer()
                    checked = true
                }
            }

            Text {
                anchors {
                    right: parent.left
                    rightMargin: 10
                    bottom: parent.bottom
                }
                text: "Enable human"
                color: "white"
                opacity: humanPlayerCheckbox.opacity
                antialiasing: false
                renderType: Text.NativeRendering
            }
        }

        Checkbox {
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 10
            checked: Settings.getValue(Settings.EnableEffects, true)

            function handleValue() {
                // Force kill all particles
                particleSystem.stop()
                if (checked) {
                    particleSystem.start()
                }
                particleSystem.visible = checked
            }

            Component.onCompleted: handleValue()

            onClicked: {
                checked = !checked
                Settings.setValue(Settings.EnableEffects, checked)
                handleValue()
            }

            Text {
                anchors.right: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 10
                text: "Enable effects"
                color: "white"
                antialiasing: false
                renderType: Text.NativeRendering
            }
        }
    }

    Rectangle {
        id: playerCount
        anchors {
            bottom: playerList.top
            left: playerList.left
            right: playerList.horizontalCenter
            bottomMargin: 10
            rightMargin: 5
        }
        height: 40
        color: "#7f000000"
        border.width: 4
        border.color: "white"

        Text {
            color: "white"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.margins: 10
            text: "Connected: " + GameManager.players.length + "/" + GameManager.maxPlayers;
            antialiasing: false
            renderType: Text.NativeRendering
        }
    }


    // List of players
    Rectangle {
        id: playerList
        color: "#7f000000"
        anchors.centerIn: parent
        width: 600
        height: 400
        anchors.bottomMargin: 100
        border.color: "white"
        border.width: 4

        Column {
            id: userListColumn
            anchors.fill: parent
            anchors.margins: 20
            spacing: 10
            Repeater {
                model: GameManager.players
                delegate: Item {
                    height: 40
                    width: userListColumn.width
                    Image {
                        source: "qrc:///sprites/players/player" + modelData.id + "-" + modelData.direction + "-" + "0" + ".png"
                        width: 30
                        height: width
                        anchors {
                            left: parent.left
                            verticalCenter: parent.verticalCenter
                        }

                        Text {
                            anchors {
                                left: parent.right
                                leftMargin: 20
                                verticalCenter: parent.verticalCenter
                            }
                            color: "white"
                            font.pixelSize: 32
                            font.family: "Perfect DOS VGA 437 Win"
                            text: modelData.name
                            antialiasing: false
                            renderType: Text.NativeRendering
                        }
                    }

                    Button {
                        visible: !modelData.isHuman()
                        anchors.right: parent.right
                        width: 60
                        height: 30
                        text: "Kick"
                        fontSize: 10
                        onClicked: GameManager.kick(index)
                    }
                }
            }
        }
    }

    // Start button
    Button {
        id: startButton
        anchors {
            top: playerList.bottom
            right: playerList.right
            left: playerList.left
            topMargin: 10
        }
        height: 60
        text: "Start game"
        active: (GameManager.players.length > 0)
        onClicked: {
            if (GameManager.players.length < 1) {
                return
            }
            GameManager.startGame()
        }
    }



    Button {
        id: quitButton
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            bottomMargin: 10
        }
        height: 50
        width: 200
        text: "Quit"
        onClicked: {
            Qt.quit()
        }
    }
}

