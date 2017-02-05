import QtQuick 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Particles 2.0
import org.gathering.ghostly 1.0

Item {
    id: charactersContainer

    property int spriteSize
    
    Repeater {
        model: GameManager.players
        delegate: Item {
            CharacterSprite {
                id: playerSprite
                spriteBase: "players/player" + modelData.id + "-" + modelData.direction
                name: modelData.name
                x: modelData.x * charactersContainer.spriteSize
                y: modelData.y * charactersContainer.spriteSize
                
                alive: modelData.alive
                onAliveChanged: {
                    if (!alive && GameManager.roundRunning && deathEmitter.item) {
                        deathEmitter.item.pulse(250);
                    }
                }
                
                Age {
                    anchors.fill: parent
                    system: particles
                    once: true
                    lifeLeft: 500
                }
                
                // Have to wrap these in loaders because crashy shit
                Loader {
                    id: deathEmitter
                    active: main.effectsEnabled
                    anchors.fill: parent
                    sourceComponent: Component {
                        // Black wave
                        Emitter {
                            emitRate: 1000
                            enabled: false
                            lifeSpan: 1000
                            lifeSpanVariation: 500
                            velocity: AngleDirection{ magnitude: 100; magnitudeVariation: 10; angleVariation: 360}
                            size: 128

                            shape: EllipseShape { }

                            group: "Monster"
                            system: particles
                        }
                    }
                }
            }
            
            Loader {
                active: main.effectsEnabled
                sourceComponent: Component {
                    Emitter {
                        id: playerTrailEmitter
                        x: playerSprite.x
                        y: playerSprite.y
                        Behavior on x { enabled: GameManager.roundRunning; SmoothedAnimation { velocity: GameManager.getTickRate() * playerSprite.width } }
                        Behavior on y { enabled: GameManager.roundRunning; SmoothedAnimation { velocity: GameManager.getTickRate() * playerSprite.height } }
                        emitRate: 500
                        width: playerSprite.width
                        height: width
                        size: 32

                        lifeSpan: 5000
                        enabled: playerSprite.alive && GameManager.roundRunning
                        velocity: AngleDirection{ magnitude: 1; magnitudeVariation: 1; angleVariation: 360}
                        velocityFromMovement: 4
                        system: particles
                        group: particles.particleGroups[model.modelData.id]
                        shape: EllipseShape { }
                    }
                }
            }
        }
    }
    
    CharacterSprite {
        id: monsterSprite
        
        property QtObject monsterData: GameManager.monster()
        x: monsterData.x * charactersContainer.spriteSize
        y: monsterData.y * charactersContainer.spriteSize
        
        spriteBase: "monster/" +  monsterData.direction
        alive: monsterData.active
        
    }

    Loader {
        active: main.effectsEnabled
        sourceComponent: Component {
            Emitter {
                id: monsterTrailEmitter
                x: monsterSprite.x - monsterSprite.width / 4
                y: monsterSprite.y - monsterSprite.height / 4
                Behavior on x { enabled: GameManager.roundRunning; SmoothedAnimation { velocity: GameManager.getTickRate() * monsterSprite.width } }
                Behavior on y { enabled: GameManager.roundRunning; SmoothedAnimation { velocity: GameManager.getTickRate() * monsterSprite.height } }
                emitRate: 50
                width: monsterSprite.width * 2
                height: width
                size: 128

                lifeSpan: 60000
                enabled: monsterSprite.alive /*&& monsterSprite.moving &&*/ && GameManager.gameRunning
                velocity: AngleDirection{ magnitude: 1; magnitudeVariation: 1; angleVariation: 360}
                velocityFromMovement: 1
                system: particles
                group: "Monster"
                shape: EllipseShape { }
            }
        }
    }
}
