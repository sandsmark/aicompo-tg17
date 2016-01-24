import QtQuick 2.0
import QtQuick.Particles 2.0
import QtGraphicalEffects 1.0

Item {
    id: missile
    smooth: false
    width: (parent == null || parent.scaleSize === undefined) ? 0 : parent.scaleSize / 30
    height: width

    Image {
        id: image
        anchors.fill: parent
        source: modelData.energy < 75 ?
                    (modelData.energy < 40 ?
                         "qrc:/sprites/missile-empty.png" :
                         "qrc:/sprites/missile-half.png") :
                    "qrc:/sprites/missile-full.png"
        visible: false
        smooth: false
    }
    ColorOverlay {
        anchors.fill: image
        source: image
        color: playerColors[modelData.owner()]
    }

    rotation: modelData.rotation + 90
    property bool alive: modelData.alive

    Emitter {
        id: trailEmitter
        anchors.centerIn: parent
        emitRate: modelData.energy / 10
        lifeSpan: 2000
        lifeSpanVariation: 1000
        enabled: true
        velocity: AngleDirection{ angle: modelData.rotation; magnitude: 100; angleVariation: 15}
        size: 24
        sizeVariation: 16
        system: missileParticles
        velocityFromMovement: 0.9
    }
}
