import QtQuick 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Particles 2.0
import org.gathering.ghostly 1.0

Grid {
    id: gameGrid

    property int maxSize: Math.min(parent.height, parent.width) - 20
    property int maxTiles: Math.max(Map.width, Map.height)

    property int tileSize: maxSize / maxTiles


    height: maxSize * Map.height / maxTiles
    width: maxSize * Map.width / maxTiles
    
    
    columns: Map.width
    rows: Map.height

    Connections {
        target: Map
        onMapChanged: {
            rowRepeater.model = 0
            rowRepeater.model = Map.height
        }
    }
    
    Repeater {
        id: rowRepeater
        model: Map.height
        delegate: Repeater {
            id: columnRepeater
            property int spriteY: modelData
            model: Map.width
            delegate: Item {
                width: gameGrid.tileSize
                height: width
                property int spriteX: modelData
                Image {
                    width: parent.width / 2
                    height: width
                    anchors {
                        top: parent.top
                        left: parent.left
                    }
                    
                    smooth: false
                    source: "qrc:///sprites/map/" + Map.tileSprite(spriteX, spriteY, 0) + ".png"
                }
                Image {
                    width: parent.width / 2
                    height: width
                    anchors {
                        top: parent.top
                        right: parent.right
                    }
                    mirror: true
                    
                    smooth: false
                    source: "qrc:///sprites/map/" + Map.tileSprite(spriteX, spriteY, 1) + ".png"
                }
                Image {
                    width: parent.width / 2
                    height: width
                    anchors {
                        bottom: parent.bottom
                        left: parent.left
                    }
                    rotation: 180
                    mirror: true
                    
                    smooth: false
                    source: "qrc:///sprites/map/" + Map.tileSprite(spriteX, spriteY, 2) + ".png"
                }
                Image {
                    width: parent.width / 2
                    height: width
                    anchors {
                        bottom: parent.bottom
                        right: parent.right
                    }
                    rotation: 180
                    
                    smooth: false
                    source: "qrc:///sprites/map/" + Map.tileSprite(spriteX, spriteY, 3) + ".png"
                }
                
                Image {
                    id: pellet
                    anchors.fill: parent
                    source: sprite == "" ? "" : "qrc:///sprites/map/" + sprite + ".png"
                    property string sprite: Map.powerupSprite(spriteX, spriteY)
                    smooth: false
                    Connections {
                        target: Map
                        onPowerupVisibleChanged: {
                            if (x != spriteX || y != spriteY) {
                                return
                            }
                            pellet.visible = visible
                        }
                        onPowerupChanged: {
                            if (x != spriteX || y != spriteY) {
                                return
                            }
                            
                            pellet.sprite = Map.powerupSprite(spriteX, spriteY)
                            pellet.visible = true
                        }
                    }
                }
            }
        }
    }
}
