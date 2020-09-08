import QtQuick 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Particles 2.0
import org.gathering.ghostly 1.0

ParticleSystem {
    id: particles
    
    // Less ugly
    onWidthChanged: restart()
    onHeightChanged: restart()
    Connections {
        target: GameManager

        function onRoundStarted() {
            particles.reset();
        }
    }
    
    property var particleGroups: ["Player1", "Player2", "Player3", "Player4"]
    
    ImageParticle {
        opacity: 0.1
        source: "qrc:///sprites/star.png"
        color: playerColors[0]
        groups: ["Player1"]
    }
    ImageParticle {
        opacity: 0.1
        source: "qrc:///sprites/star.png"
        color: playerColors[1]
        groups: ["Player2"]
    }
    ImageParticle {
        opacity: 0.1
        source: "qrc:///sprites/star.png"
        color: playerColors[2]
        groups: ["Player3"]
    }
    ImageParticle {
        opacity: 0.1
        source: "qrc:///sprites/star.png"
        color: playerColors[3]
        groups: ["Player4"]
    }
    ImageParticle {
        source: "qrc:///sprites/star.png"
        alpha: 0.9
        alphaVariation: 0.1
        color: "black"
        groups: ["Monster"]
    }
    ImageParticle {
        source: "qrc:///sprites/star.png"
        color: "white"
        groups: ["Explosion", "Logo"]
    }
}
