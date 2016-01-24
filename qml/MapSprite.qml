import QtQuick 2.0
import com.iskrembilen.turnonme 1.0

Rectangle {
    property int type: -1

    onTypeChanged: {
        if (type === Tile.Grass) {
            image.source = "qrc:/sprites/tiles/grass/grass-" + Math.floor(Math.random() * 4) + ".png"
        } else if (type === Tile.Floor) {
            image.source = "qrc:/sprites/tiles/floor/floor-" + Math.floor(Math.random() * 3) + ".png"
        } else if (type === Tile.Wall) {
            image.source = "qrc:/sprites/tiles/brickwall/brickwall-" + Math.floor(Math.random() * 3) + ".png"
        } else if (type === Tile.Stone) {
            image.source = "qrc:/sprites/tiles/stone/stone-" + Math.floor(Math.random() * 4) + ".png"
        } else if (type == Tile.Debris) {
            image.source = "qrc:/sprites/tiles/pebbles/pebbles-" + Math.floor(Math.random() * 2) + ".png"
        } else {
            console.log("TILE: Unknown tile type:" + type)
        }
    }

    function explode() {
        explosionAnimation.running = true
    }

    Image {
        id: image
        smooth: false
        height: width * 1.25
        width: parent.width
    }

    Image {
        id: explosion
        smooth: false
        height: image.width
        width: height
        source: "qrc:/sprites/explosion/flame-" + Math.floor(Math.random() * 5) + ".png"
        onOpacityChanged: source = "qrc:/sprites/explosion/flame-" + Math.floor(Math.random() * 5) + ".png"
        opacity: 0
        NumberAnimation on opacity {
            id: explosionAnimation
            running: false
            duration: 200
            from: 1
            to: 0
        }
    }
}
