import QtQuick 2.0

Image {
    smooth: false
    width: 64
    height: 64
    property QtObject bombData
    property int bombState: 0
    source: "qrc:/sprites/bomb/bomb-" + bombData.state + ".png"
    onBombDataChanged: {
        console.log(bombData.position + " foo " + parent.x)
        x = bombData.position.x * 64;
        y = bombData.position.y * 64;
        bombState = bombData.state
    }
}
