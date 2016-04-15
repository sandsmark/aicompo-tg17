import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Particles 2.0

Item {
    id: playerSprite

    property int playerId

    x: targetX
    y: targetY

    height: playingField.tileSize
    width: height

    property real targetX: 0
    property real targetY: 0
    property real playerX: modelData.x * width
    property real playerY: modelData.y * height
    property real lastPlayerX: 0
    property real lastPlayerY: 0

    onPlayerXChanged: {
        if (Math.abs(playerX - lastPlayerX) > width * 2) {
            targetX = playerX
        } else {
            xAnimation.from = lastPlayerX
            xAnimation.to = playerX
            xAnimation.restart()
        }

        lastPlayerX = playerX
    }

    onPlayerYChanged: {
        if (Math.abs(playerY - lastPlayerY) > height * 2) {
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

    property int walkCycle: 0
    Timer {
        interval: 100
        repeat: true
        running: true
        onTriggered: {
            if (walkCycle) {
                walkCycle = 0
            } else {
                walkCycle = 1
            }
        }

    }

    Image {
        id: image
        anchors.fill: parent
        source: "qrc:///sprites/players/player" + modelData.id + "-" + modelData.direction + "-" + walkCycle + ".png"
        smooth: false
        visible: true
        opacity: alive
        Behavior on opacity {
            NumberAnimation {
                duration: 500
            }
        }

    }


    Emitter {
        id: deathEmitter
        anchors.fill: parent
        emitRate: 1000
        lifeSpan: 2000
        lifeSpanVariation: 1000
        enabled: false
        velocity: AngleDirection{magnitude: 1; magnitudeVariation: 4; angleVariation: 360}
        size: 2
        sizeVariation: 4

        shape: MaskShape {
            source: image.source
        }
        group: particleSystem.particleGroups[playerId]
        system: particleSystem
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
        anchors.top: parent.bottom
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
        emitRate: 50

        lifeSpan: 10000
        lifeSpanVariation: 1000
        enabled: alive && (yAnimation.running || xAnimation.running)
        velocity: AngleDirection{ magnitude: 2; magnitudeVariation: 1; angleVariation: 360}
        size: 1
        system: particleSystem
        group: particleSystem.particleGroups[playerId]
        shape: EllipseShape { }
    }
}
