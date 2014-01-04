import QtQuick 2.0

Rectangle {
    //anchors.fill: parent
    color: "black"
    width: 1000
    height: 1000

    Grid {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        //anchors.centerIn: parent
        width: rows * 64
        height: columns * 64

        rows: map.height
        columns: map.width
        Component.onCompleted: console.log("rows: " + rows + " columns: " + columns)
        Repeater {
            id: repeater
            model: tiles
    //        Component.onCompleted: console.log("height: " + tiles.length)
            delegate: QmlTile {
                type: model.modelData.type
                border.width: 1
            }
        }
    }
}
