import QtQuick 2.0

Rectangle {
    //anchors.fill: parent
    color: "black"

    signal userMove(string direction)

    Keys.onDownPressed: userMove("down")
    Keys.onUpPressed: userMove("up")
    Keys.onRightPressed: userMove("right")
    Keys.onLeftPressed: userMove("left")
    Keys.onSpacePressed: userMove("dropBomb")

    focus: true

    // Map background grid
    Grid {
        id: playingField
        objectName: "playingField"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        //anchors.centerIn: parent
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
