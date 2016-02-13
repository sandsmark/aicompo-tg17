import QtQuick 2.0
import QtQuick.Particles 2.0
import QtGraphicalEffects 1.0

Item {
    id: missile
    smooth: false
    width: main.scaleSize / 30
    height: width

    property var missileData
    property int energy: (missileData === null) ? 0 : missileData.energy
    rotation: (missileData === null) ? 0 : missileData.rotation + 90

    onMissileDataChanged: {
        if (missileData === null) {
            destroy()
        }
    }

    x: main.width / 2 + targetX * main.width / 2 - width / 2
    y: main.height / 2 + targetY * main.height / 2 - height / 2


    property real targetX: 0
    property real targetY: 0
    property real missileX: (missileData === null) ? 0 : missileData.position.x
    property real missileY: (missileData === null) ? 0 : missileData.position.y
    property real lastMissileX: 0
    property real lastMissileY: 0

    onMissileXChanged: {
        if (Math.abs(missileX - lastMissileX) > 0.1) {
            targetX = missileX
        } else {
            xAnimation.from = lastMissileX
            xAnimation.to = missileX
            xAnimation.restart()
        }

        lastMissileX = missileX
    }

    onMissileYChanged: {
        if (Math.abs(missileY - lastMissileY) > 0.1) {
            targetY = missileY
        } else {
            yAnimation.from = lastMissileY
            yAnimation.to = missileY
            yAnimation.restart()
        }

        lastMissileY = missileY
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
        anchors.fill: parent
        source: missile.energy < 75 ?
                    (missile.energy < 40 ?
                         "qrc:/sprites/missile-empty.png" :
                         "qrc:/sprites/missile-half.png") :
                    "qrc:/sprites/missile-full.png"
        visible: false
        smooth: false
    }
    //onRotationChanged: console.log(rotation)
    ColorOverlay {
        anchors.fill: image
        source: image
        color: missileData === null ? "transparent" : playerColors[missileData.owner()]
//        rotation: missile.rotation
    }

    Emitter {
        id: trailEmitter
        anchors.centerIn: parent
        width: parent.width / 2
        height: width
        emitRate: missile.energy > 10 ? 500 : 10
        lifeSpan: 500
        lifeSpanVariation: 90
        enabled: true
        group: (missileData === null) ? "" : particleSystem.particleGroups[missileData.owner()]
        velocity: AngleDirection {
            angle: missile.rotation + 90
            magnitude: missile.energy > 10 ? 200 : 0
            magnitudeVariation: missile.energy > 10 ? 100 : 10
            angleVariation: 5
        }
        size: 8
        sizeVariation: 4
        system: particleSystem
        //velocityFromMovement: 10
        endSize: 20
        shape: EllipseShape {
        }
    }

//    RotationAnimation on rotation {
//        from: 0
//        to: 360
//        duration: 5000
//        loops: Animation.Infinite
//    }
}
