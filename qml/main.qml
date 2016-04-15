import QtQuick 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Particles 2.0
import org.gathering.ghostly 1.0

Rectangle {
    id: main

    color: "black"

    height: 1200
    width: 1600

    property int scaleSize: (width < height) ? width : height

    property var playerColors: [
        "#c0ff400f", // red
        "#c0400fff", // blue
        "#c040ff0f", // green
        "#c0f0ff40", // yellow
        "#c0f601ff", // pink
        "#c0ffa201", // brown
        "#c001ffde", // cyan
        "#c0a801ff"  // purple
    ]

    signal userMove(string direction)

    Keys.onUpPressed: userMove("up")
    Keys.onDownPressed: userMove("down")
    Keys.onRightPressed: userMove("right")
    Keys.onLeftPressed: userMove("left")
    Keys.onEscapePressed: {
        GameManager.stopGame();
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_F5) {
            GameManager.endRound();
            return true;
        } else if (event.key === Qt.Key_P) {
            GameManager.togglePause()
            pauseText.visible = !pauseText.visible
            pauseAnimation.restart()
            return true;
        } else if (event.key === Qt.Key_Period) {
            userMove("SEEKING")
        } else if (event.key === Qt.Key_Comma) {
            userMove("MINE")
        } else if (event.key === Qt.Key_M) {
            userMove("MISSILE")
        }

        return false;
    }


    ColorAnimation on color {
        id: backgroundColorAnimation
        from: "red"
        to: "black"
        duration: 1000
        running: false
    }

    Connections {
        target: GameManager
        onRoundOver: {
            backgroundColorAnimation.restart()
        }
    }

    Text {
        id: timerText
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }

        font.pointSize: 20
        text: GameManager.timeElapsed
        color: "white"
    }

    Rectangle {
        id: pelletBar
        anchors {
            top: timerText.bottom
            horizontalCenter: parent.horizontalCenter
        }
        height: 20
        width: 500
        color: "transparent"
        border.color: "white"
        border.width: 4

        Rectangle {
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }
            width: parent.width * Math.max(Map.pelletsLeft, 1) / Map.totalPellets
        }
    }


    Rectangle {
        color: "#aa000000"
        id: playingField
        anchors.centerIn: parent
        property int maxSize: Math.min(parent.height, parent.width) - 20
        property int maxTiles: Math.max(Map.width, Map.height) - 1
        height: maxSize * Map.height / maxTiles
        width: maxSize * Map.width / maxTiles

        property int tileSize: maxSize / maxTiles

        opacity: startScreen.visible ? 0.1 : 1
        Behavior on opacity {
            NumberAnimation { duration: 500 }
        }

        Grid {
            anchors.fill: parent
            columns: Map.width
            rows: Map.height

            Repeater {
                anchors.horizontalCenter: parent.horizontalCenter
                model: Map.height
                delegate: Repeater {
                    property int spriteY: modelData
                    model: Map.width
                    delegate: Item {
                        width: playingField.tileSize
                        height: width
                        property int spriteX: modelData
                        Image {
                            width: parent.width / 2
                            height: width
                            anchors {
                                top: parent.top
                                left: parent.left
                            }

                            smooth: false
                            source: "qrc:///sprites/map/" + Map.tileSprite(spriteX, spriteY, 0) + ".png"
                        }
                        Image {
                            width: parent.width / 2
                            height: width
                            anchors {
                                top: parent.top
                                right: parent.right
                            }
                            mirror: true

                            smooth: false
                            source: "qrc:///sprites/map/" + Map.tileSprite(spriteX, spriteY, 1) + ".png"
                        }
                        Image {
                            width: parent.width / 2
                            height: width
                            anchors {
                                bottom: parent.bottom
                                left: parent.left
                            }
                            rotation: 180
                            mirror: true

                            smooth: false
                            source: "qrc:///sprites/map/" + Map.tileSprite(spriteX, spriteY, 2) + ".png"
                        }
                        Image {
                            width: parent.width / 2
                            height: width
                            anchors {
                                bottom: parent.bottom
                                right: parent.right
                            }
                            rotation: 180

                            smooth: false
                            source: "qrc:///sprites/map/" + Map.tileSprite(spriteX, spriteY, 3) + ".png"
                        }

                        Image {
                            id: pellet
                            anchors.fill: parent
                            source: "qrc:///sprites/map/" + Map.powerupSprite(spriteX, spriteY) + ".png"
                            smooth: false
                            Connections {
                                target: Map
                                onPowerupChanged: {
                                    if (x != spriteX || y != spriteY) {
                                        return
                                    }
                                    pellet.source = "qrc:///sprites/map/" + Map.powerupSprite(spriteX, spriteY) + ".png"
                                }
                            }
                        }
                    }
                }
            }
        }


        // Create player sprites
        Repeater {
            model: GameManager.players
            delegate: PlayerSprite {
                playerId: model.modelData.id
            }
        }
    }

    Item {
        id: gameArea
        anchors.fill: parent

        Text {
            id: pauseText
            anchors.top: parent.top
            anchors.right: parent.right
            text: "PAUSED"
            color: "white"
            font.bold: true
            font.pointSize: 40
            opacity: 0.7
            visible: false
            z: 10


            Text {
                id: pauseOverlay
                anchors.top: parent.top
                anchors.right: parent.right
                color: "white"
                text: parent.text
                visible: opacity > 0
                font.bold: true

                ParallelAnimation {
                    id: pauseAnimation
                    NumberAnimation {
                        target: pauseOverlay
                        property: "opacity"
                        from: 1
                        to: 0
                        duration: 200
                    }
                    NumberAnimation {
                        target: pauseOverlay
                        property: "font.pointSize"
                        from: 40
                        to: 200
                        duration: 300
                    }
                }
            }
        }

        Text {
            id: roundsPlayed
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            text: "round: " + (GameManager.roundsPlayed + 1) + "/" + GameManager.maxRounds
            color: "#7f000000"
            font.bold: true
            style: Text.Outline
            styleColor: "white"
            font.pointSize: 40
            z: 10
            opacity: (GameManager.roundsPlayed < GameManager.maxRounds) ? 0.7 : 0
        }

        ParticleSystem {
            id: particleSystem
            anchors.fill: parent

            property var particleGroups: ["Player1", "Player2", "Player3", "Player4"]

            ImageParticle {
                opacity: 0.5
                source: "qrc:///sprites/rect.png"
                alpha: 0.1
                alphaVariation: 0.1
                colorVariation: 0.3
                color: playerColors[0]
                groups: ["Player1"]
            }
            ImageParticle {
                opacity: 0.5
                source: "qrc:///sprites/rect.png"
                alpha: 0.1
                alphaVariation: 0.1
                colorVariation: 0.3
                color: playerColors[1]
                groups: ["Player2"]
            }
            ImageParticle {
                opacity: 0.5
                source: "qrc:///sprites/rect.png"
                alpha: 0.1
                alphaVariation: 0.1
                colorVariation: 0.3
                color: playerColors[2]
                groups: ["Player3"]
            }
            ImageParticle {
                opacity: 0.5
                source: "qrc:///sprites/rect.png"
                alpha: 0.1
                alphaVariation: 0.1
                colorVariation: 0.3
                color: playerColors[3]
                groups: ["Player4"]
            }
            ImageParticle {
                opacity: 0.5
                source: "qrc:///sprites/rect.png"
                alpha: 0.1
                alphaVariation: 0.9
                color: "white"
                groups: ["Explosion", "Logo"]
            }
        }


        Emitter {
            id: crashEmitter
            anchors.centerIn: parent
            width: 10
            height: 10
            emitRate: 1000
            lifeSpan: 250
            lifeSpanVariation: 100
            enabled: false
            velocity: AngleDirection{magnitude: 128; angleVariation: 360}
            size: 16
            sizeVariation: 16
            system: particleSystem
            group: "Explosion"

            Connections {
                target: GameManager
                onExplosion: {
                    crashEmitter.burst(500, main.width / 2 + position.x * main.width / 2, main.height/ 2 + position.y * main.height/ 2)
                }
            }
        }


//        // List of names of players
//        Column {
//            anchors.left: parent.left
//            anchors.top: parent.top
//            height: 100
//            width: 100
//            Repeater {
//                model: GameManager.players
//                delegate: Image {
//                    source: modelData.spritePath
//                    height: 45
//                    width: height


//                    Rectangle {
//                        anchors.left: parent.right
//                        anchors.top: parent.top
//                        anchors.bottom: parent.bottom
//                        color: playerColors[modelData.id]
//                        width: 500 * modelData.energy / 1000
//                        Behavior on width { NumberAnimation { duration: 100; } }
//                    }
//                    Text {
//                        id: playerName
//                        anchors.verticalCenter: parent.verticalCenter
//                        anchors.left: parent.right
//                        width: 500
//                        text: modelData.name + " (" + modelData.wins + ")"
//                        color: "white"
//                        style: Text.Outline
//                        styleColor: "black"
//                        font.family: "Aldrich"
//                        font.pointSize: 20
//                        font.strikeout: !modelData.alive
//                    }
//                }
//            }
//        }

        Rectangle {
            id: startCountdown
            anchors.centerIn: parent
            height: parent.height / 2
            width: height / 3
            color: "#a0000000"
            border.color: "white"
            border.width: 2

            opacity: countdownTimer.running ? 1 : 0

            Behavior on opacity {
                NumberAnimation {
                    duration: 500
                }
            }

            Rectangle {
                id: countdownFirst
                color: "yellow"
                opacity: 0
                width: parent.width
                height: width
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Rectangle {
                id: countdownSecond
                color: "yellow"
                opacity: 0
                width: parent.width
                height: width
                anchors {
                    verticalCenter: parent.verticalCenter
                    horizontalCenter: parent.horizontalCenter
                }
            }


            Rectangle {
                id: countdownThird
                color: "green"
                opacity: 0
                width: parent.width
                height: width
                anchors {
                    bottom: parent.bottom
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Connections {
                target: GameManager
                onShowCountdown: {
                    countdownFirst.opacity = 0
                    countdownSecond.opacity = 0
                    countdownThird.opacity = 0
                    countdownTimer.progress = 0
                    countdownTimer.restart()
                }
                onGameRunningChanged: {
                    countdownTimer.stop()
                }
            }

            Timer {
                id: countdownTimer
                interval: 250
                running: false
                repeat: true
                property int progress: 0

                onTriggered: {
                    if (progress === 0) {
                        countdownFirst.opacity = 0.5
                    } else if (progress === 1) {
                        countdownSecond.opacity = 0.5
                    } else {
                        countdownThird.opacity = 0.5
                        countdownTimer.stop()
                    }

                    progress++
                }
            }
        }
    }

    BrightnessContrast {
        id: gameFilter
        source: gameArea
        anchors.fill: gameArea
        opacity: 0
        NumberAnimation on opacity {
            running: false
            id: blurAnimation
            duration: 250
            from: 0.7
            to: 0
        }
        brightness: 0.9
        contrast: 1
    }


    focus: true


    EndScreen {
        opacity: (GameManager.roundsPlayed >= GameManager.maxRounds) ? 1 : 0
        objectName: "endScreen"
    }

    StartScreen {
        id: startScreen
        opacity: GameManager.gameRunning ? 0 : 1
    }

    Text {
        id: aboutText
        anchors.bottom: buildId.top
        anchors.right: parent.right
        anchors.margins: 10
        color: "white"
        text: "code:martin^remarkable"
        font.pointSize: 10
        opacity: 0.5
    }

    Text {
        id: buildId
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        color: "white"
        text: GameManager.version()
        font.pointSize: 10
        opacity: 0.3
    }
}
