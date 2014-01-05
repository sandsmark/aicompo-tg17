import QtQuick 2.0

Rectangle {
    color: "black"

    signal userMove(string direction)

    Keys.onDownPressed: userMove("DOWN")
    Keys.onUpPressed: userMove("UP")
    Keys.onRightPressed: userMove("RIGHT")
    Keys.onLeftPressed: userMove("LEFT")
    Keys.onSpacePressed: userMove("BOMB")

    focus: true

    // Map background grid
    Grid {
        id: playingField
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

    // List of player sprites
    Repeater {
        model: players
        delegate: PlayerSprite {
            playerId: model.modelData.id
            x: playingField.x + model.modelData.position.x * 64
            y: playingField.y + model.modelData.position.y * 64
        }
    }

    EndScreen {
        opacity: 0
        objectName: "endScreen"
    }
}
