import QtQuick 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Particles 2.0
import org.gathering.ghostly 1.0

Item {
    id: playingField

    property int maxSize: Math.min(height, width) - 20
    property int maxTiles: Math.max(Map.width, Map.height)

    property int tileSize: maxSize / maxTiles

    opacity: startScreen.visible ? 0.1 : 1
    Behavior on opacity {
        NumberAnimation { duration: 500 }
    }

    Grid {
        id: gameGrid

        height: maxSize * Map.height / maxTiles
        width: maxSize * Map.width / maxTiles

        anchors.centerIn: parent

        columns: Map.width
        rows: Map.height

        Repeater {
            model: Map.height
            delegate: Repeater {
                property int spriteY: modelData
                model: Map.width
                delegate: Item {
                    width: playingField.tileSize
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
                        source: sprite === "" ? "" : "qrc:///sprites/map/" + sprite + ".png"
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

                                pellet.source = "qrc:///sprites/map/" + Map.powerupSprite(spriteX, spriteY) + ".png"
                                pellet.visible = true
                            }
                        }
                    }
                }
            }
        }
    }
    
    
    // Create player sprites
    Repeater {
        model: GameManager.players
        delegate: PlayerSprite {
            id: playerSprite
            x: targetX + gameGrid.x
            y: targetY + gameGrid.y
            playerX: modelData.x * width
            playerY: modelData.y * height
            spriteBase: "players/player" + modelData.id + "-" + modelData.direction
            name: modelData.name

            alive: modelData.alive
            onAliveChanged: {
                if (!alive) {
                    deathEmitter.pulse(500);
                    blurAnimation.restart()
                }
            }

            Emitter {
                id: deathEmitter
                anchors.fill: parent
                emitRate: 1000
                lifeSpan: 2000
                lifeSpanVariation: 1000
                enabled: false
                velocity: AngleDirection{magnitude: 1; magnitudeVariation: 4; angleVariation: 360}
                size: 2
                sizeVariation: 4

                shape: MaskShape {
                    source: playerSprite.spriteImageSource
                }
                group: particleSystem.particleGroups[model.modelData.id]
                system: particleSystem
            }

            Emitter {
                id: trailEmitter
                anchors.fill: parent
                emitRate: 50

                lifeSpan: 10000
                lifeSpanVariation: 1000
                enabled: playerSprite.alive && playerSprite.moving
                velocity: AngleDirection{ magnitude: 2; magnitudeVariation: 1; angleVariation: 360}
                size: 1
                system: particleSystem
                group: particleSystem.particleGroups[model.modelData.id]
                shape: EllipseShape { }
            }
        }
    }
    
    PlayerSprite {
        id: monsterSprite

        property QtObject monsterData: GameManager.monster()
        x: targetX + gameGrid.x
        y: targetY + gameGrid.y

        playerX: monsterData.x * width
        playerY: monsterData.y * height

        spriteBase: "monster/" +  monsterData.direction
        alive: true
    }
}
