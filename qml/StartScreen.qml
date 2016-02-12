import QtQuick 2.0
import QtQuick.Particles 2.0
import QtGraphicalEffects 1.0

Item {
    id: startScreen
    anchors.fill: parent

    visible: opacity > 0

    Behavior on opacity {
        NumberAnimation {
            easing { type: Easing.OutElastic; amplitude: 1.0; period: 0.9 }
            duration: 2000
        }
    }


    Emitter {
        lifeSpan: 4000
        emitRate: 120
        size: 12
        anchors.centerIn: parent
        enabled: parent.visible
        //! [0]

        property real petalLength: 180
        property real petalRotation: 0
        NumberAnimation on petalRotation {
            from: 0;
            to: 360;
            loops: -1;
            running: true
            duration: 24000
        }
        function convert(a) {return a*(Math.PI/180);}
        onEmitParticles: {
            for (var i=0; i<particles.length; i++) {
                var particle = particles[i];
                particle.startSize = Math.max(02,Math.min(492,Math.tan(particle.t/2)*24));
                var theta = Math.floor(Math.random() * 6.0);
                particle.red = theta == 0 || theta == 1 || theta == 2 ? 0.2 : 1;
                particle.green = theta == 2 || theta == 3 || theta == 4 ? 0.2 : 1;
                particle.blue = theta == 4 || theta == 5 || theta == 0 ? 0.2 : 1;
                theta /= 6.0;
                theta *= 2.0*Math.PI;
                theta += convert(petalRotation);//Convert from degrees to radians
                particle.initialVX = petalLength * Math.cos(theta);
                particle.initialVY = petalLength * Math.sin(theta);
                particle.initialAX = particle.initialVX * -0.5;
                particle.initialAY = particle.initialVY * -0.5;
            }
        }
        //! [0]
        group: "Petal"
        system: particleSystem
    }


    Emitter {
        anchors.top: parent.top
        anchors.bottom: titleText.top
        anchors.bottomMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        width: height
        enabled: parent.visible
        emitRate: 5000
        lifeSpan: 2000
        size: 15
        sizeVariation: 15
        //! [0]
        shape: MaskShape {
            source: "qrc:///sprites/turnon.png"
        }
        velocity: PointDirection {
            xVariation: 10
            yVariation: 10
        }
        system: particleSystem
        group: "Explosion"
        //! [0]
    }

    Text {
        id: titleText
        //anchors.top: parent.top
        anchors.bottom: settingsList.top
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        text: "TURN ON ME"
        color: "black"
        //color: "#7f000000"
        font.pointSize: 30
        font.bold: true
        style: Text.Outline
        styleColor: "white"


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

        border.width: 1
        border.color: "white"
        color: "#7f000000"

        Checkbox {
            id: humanPlayerCheckbox
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10
            opacity: (GameManager.players.length < GameManager.maxPlayers || checked) ? 1 : 0
            enabled: GameManager.players.length < GameManager.maxPlayers || checked
            onClicked: {
                if (checked) {
                    GameManager.removeHumanPlayers()
                    checked = false
                } else {
                    if (GameManager.players.length >= GameManager.maxPlayers) return;

                    GameManager.addPlayer()
                    checked = true
                }
            }

            Text {
                anchors.right: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 10
                text: "Enable human player:"
                font.pixelSize: 20
                color: "white"
                opacity: humanPlayerCheckbox.opacity
            }
        }

        Checkbox {
            id: debugCheckbox
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 10

            onClicked: {
                checked = !checked
                GameManager.setTickInterval(checked ? 10 : 50)
            }

            Text {
                anchors.right: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 10
                text: "Enable debug mode:"
                font.pixelSize: 20
                color: "white"
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
        //width: 250
        color: "#7f000000"
        border.width: 1
        border.color: "white"

        Text {
            font.pixelSize: 20
            color: "white"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.margins: 10
            text: "Connected users: " + GameManager.players.length + "/" + GameManager.maxPlayers;
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
        border.width: 1

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
                        source: modelData.spritePath
                        width: 30
                        height: width
                        anchors {
                            left: parent.left
                            verticalCenter: parent.verticalCenter
                        }

                        Text {
                            anchors.left: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            color: "white"
                            font.pixelSize: 20
                            text: modelData.name
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
            GameManager.resetScores()
            GameManager.startRound()
        }
    }



    Button {
        id: quitButton
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            bottomMargin: 10
        }
        height: 40
        width: 200
        text: "Quit"
        onClicked: {
            Qt.quit()
        }
    }
}

