import QtQuick 2.0

Grid {
    objectName: "playingField"
    //anchors.horizontalCenter: parent.horizontalCenter
    //anchors.verticalCenter: parent.verticalCenter
    anchors.centerIn: parent
    width: rows * 64
    height: columns * 64

    rows: map.height
    columns: map.width

    Repeater {
        id: repeater
        model: map.tiles
        delegate: MapSprite {
            type: model.modelData.type
            border.width: 1
        }
    }

    Timer {
        property int count: 0
        id: explosionTimer
        interval: 50
        repeat: true
        running: false
        onTriggered: {
            playingField.anchors.horizontalCenterOffset = Math.random() * 10
            playingField.anchors.verticalCenterOffset = Math.random() * 10
            if (count > 3) {
                running = false
            }
            count++;
        }
    }
    Connections {
        target: map
        onExplosionAt: {
            explosionTimer.count = 0
            explosionTimer.running = true
        }
    }
}
