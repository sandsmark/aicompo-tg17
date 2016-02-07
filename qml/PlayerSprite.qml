import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Particles 2.0

Item {
    id: playerSprite
    rotation: modelData.rotation + 90

    property int playerId
    property string command: modelData.lastCommand

    ParticleSystem {
        id: accelerationParticles
        anchors.fill: parent
        ImageParticle {
            source: "qrc:///sprites/star.png"
            alpha: 0.1
            alphaVariation: 0.1
            color: playerColors[playerId]
//            colorVariation: 0.3
            //color: "#01ffffff"
        }
    }

    x: main.width / 2 + targetX * main.width / 2 - width / 2
    y: main.height / 2 + targetY * main.height / 2 - height / 2

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

//    Behavior on playerX {
//        SmoothedAnimation {
//            velocity: 60/modelData.velocityX
////            easing.type: Easing.Linear
//        }
//    }

//    Behavior on playerY {
//        SmoothedAnimation {
//            velocity: 60/modelData.velocityY
//            //velocity: 60
////            easing.type: Easing.Linear
//        }
//    }

    Image {
        id: image
        anchors.centerIn: parent
        width: parent.width // opacity
        height: parent.height // opacity
        source: modelData.spritePath//"qrc:/sprites/players/player" + playerId + ".png"
        opacity: alive ? 1 : 0
        smooth: false

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.Linear
                duration: 1000
            }
        }

        Behavior on rotation {
            SmoothedAnimation { duration: 200 }
        }

        Emitter {
            id: accelerationEmitter
            x: parent.width/2 - parent.width / 4
            y: parent.height /2 - parent.height / 4
            emitRate: 2000
            lifeSpan: 150
            lifeSpanVariation: 100
            enabled: false
            velocity: AngleDirection{ angle: 90; magnitude: 1000; angleVariation: 15}
            size: 50
            sizeVariation: 5
            system: accelerationParticles
            width: parent.width / 2
            height: parent.height /2
        }


        Rectangle {
            anchors.centerIn: parent
            height: width
            radius: width/2
            color: "transparent"
            width: modelData.energy / 100
            border.width: modelData.energy / 500
            border.color: playerColors[playerId]
//            opacity: 0.5
            //opacity: modelData.energy / 1000
        }
    }

    ParticleSystem {
        id: deathParticles
        anchors.fill: image
        ImageParticle {
            source: "qrc:///sprites/star.png"
            alpha: 0.0
            colorVariation: 0.1
            color: playerColors[playerId]
        }

        Emitter {
            id: deathEmitter
            anchors.fill: parent
            emitRate: 1000
            lifeSpan: 2000
            lifeSpanVariation: 1000
            enabled: false
            velocity: AngleDirection{magnitude: 8; angleVariation: 360}
            size: 24
            sizeVariation: 16

            shape: MaskShape {
                source: image.source
            }
        }
    }

    onCommandChanged: {
        if (command === "ACCELERATE") {
            accelerationEmitter.pulse(500)
//            trailEmitter.pulse(500)
        }
    }

    // Try to smooth out movement between ticks
//    Behavior on x {
//        enabled: true
//        NumberAnimation {
//            id: xAnimation
//            easing { type: Easing.OutQuad; amplitude: 1.0; period: 0.9 }
//            duration: 50
//        }
//    }

//    Behavior on y {
//        enabled: true
//        NumberAnimation {
//            id: yAnimation
//            easing { type: Easing.OutQuad; amplitude: 1.0; period: 0.9 }
//            duration: 50
//        }
//    }

    property bool alive: modelData.alive
    onAliveChanged: {
        if (!alive) {
            deathEmitter.pulse(500);
            blurAnimation.restart()
        }
    }

    property var particleGroups: ["Player1", "Player2", "Player3", "Player4"]
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
        system: missileParticles
        group: particleGroups[playerId]
        endSize: 0
        shape: EllipseShape { }
    }

}
