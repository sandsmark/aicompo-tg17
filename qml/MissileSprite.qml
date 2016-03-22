import QtQuick 2.0
import QtQuick.Particles 2.0
import QtGraphicalEffects 1.0

Item {
    id: missile
    smooth: false
    width: main.scaleSize / 40
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
        source: missile.energy < 40 ? "qrc:/sprites/missile-empty.png" : "qrc:/sprites/missile-full.png"
        visible: false
        smooth: false
    }

    ColorOverlay {
        anchors.fill: image
        source: image
        color: missileData === null ? "transparent" : playerColors[missileData.owner()]
    }
}
