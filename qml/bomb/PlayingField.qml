import QtQuick 2.0
import com.iskrembilen.bomb 1.0

Grid {
    id: playingField
    objectName: "playingField"
    //anchors.horizontalCenter: parent.horizontalCenter
    //anchors.verticalCenter: parent.verticalCenter
    //anchors.centerIn: parent
    //width: rows * 64
    //height: columns * 64
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.horizontalCenter: parent.horizontalCenter

    rows: map.height
    columns: map.width

    Repeater {
        anchors.horizontalCenter: parent.horizontalCenter
        id: repeater
        model: map.tiles
        delegate: MapSprite {
            z: (type == Tile.Grass) ? -1 : 0
            height: playingField.height / (playingField.rows +1)
            width: height
            type: model.modelData.type
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
            if (count > 2) {
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
