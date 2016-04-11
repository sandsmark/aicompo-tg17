import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Particles 2.0

Item {
    id: playerSprite

    property int playerId
    property string command: modelData.lastCommand

    x: main.width / 2 + targetX * main.width / 2 - width / 2
    y: main.height / 2 + targetY * main.height / 2 - height / 2

    height: playingField.tileSize + 15
//    height: main.scaleSize / 15
    width: height

    property real targetX: 0
    property real targetY: 0
    property real playerX: modelData.position.x
    property real playerY: modelData.position.y
    property real lastPlayerX: 0
    property real lastPlayerY: 0

    onPlayerXChanged: {
        if (Math.abs(playerX - lastPlayerX) > 0.1) {
            targetX = playerX
        } else {
            xAnimation.from = lastPlayerX
            xAnimation.to = playerX
            xAnimation.restart()
        }

        lastPlayerX = playerX
    }

    onPlayerYChanged: {
        if (Math.abs(playerY - lastPlayerY) > 0.1) {
            targetY = playerY
        } else {
            yAnimation.from = lastPlayerY
            yAnimation.to = playerY
            yAnimation.restart()
        }

        lastPlayerY = playerY
    }

    PropertyAnimation on targetX {
        id: xAnimation
        duration: 50
    }


    PropertyAnimation on targetY {
        id: yAnimation
        duration: 50
    }


    Image {
        id: image
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        source: "qrc:///sprites/players/player" + modelData.id + "-down-0.png"
//        source: modelData.spritePath
        smooth: false
        visible: false
    }

    Desaturate {
        anchors.fill: image
        opacity: alive ? 1 : 0
        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.Linear
                duration: 100
            }
        }
        source: image
        desaturation: 1 - (modelData.energy / 1000)

        Emitter {
            id: deathEmitter
            anchors.fill: parent
            emitRate: 1000
            lifeSpan: 2000
            lifeSpanVariation: 1000
            enabled: false
            velocity: AngleDirection{magnitude: 8; magnitudeVariation: 20; angleVariation: 360}
            size: 24
            sizeVariation: 16

            shape: MaskShape {
                source: image.source
            }
            group: particleSystem.particleGroups[playerId]
            system: particleSystem
        }
    }

    onCommandChanged: {
        if (command === "ACCELERATE") {
            accelerationEmitter.pulse(500)
        }
    }

    property bool alive: modelData.alive
    onAliveChanged: {
        if (!alive) {
            deathEmitter.pulse(500);
            blurAnimation.restart()
        }
    }

    Text {
        anchors.horizontalCenter: image.horizontalCenter
        anchors.bottom: parent.bottom
        text: modelData.name
        color: "white"
        style: Text.Outline
        styleColor: "black"
        font.family: "Aldrich"
        font.pointSize: 10
        font.strikeout: !modelData.alive
    }

    Emitter {
        id: trailEmitter
        anchors.fill: parent
        emitRate: 1000

        lifeSpan: 5000
        lifeSpanVariation: 90
        enabled: modelData.energy > 1
        velocity: AngleDirection{ magnitude: 10; magnitudeVariation: 10; angleVariation: 360}
        size: 8
        sizeVariation: 4
        system: particleSystem
        group: particleSystem.particleGroups[playerId]
        endSize: 0
        shape: EllipseShape { }
    }
}
