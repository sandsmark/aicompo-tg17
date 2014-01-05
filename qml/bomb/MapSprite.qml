import QtQuick 2.0
import com.iskrembilen.bomb 1.0


Rectangle {
    height: 64
    width: 64
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

    Image {
        id: image
        smooth: false
        height: 80
        width: 64
    }
}
