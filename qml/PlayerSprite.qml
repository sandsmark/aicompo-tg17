import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Particles 2.0

Item {
    id: playerSprite

    property real playerX
    property real playerY
    property string spriteBase
    property bool alive
    property string name: ""

    height: playingField.tileSize
    width: height

    property real targetX: 0
    property real targetY: 0
    property real lastPlayerX: 0
    property real lastPlayerY: 0
    property bool moving: yAnimation.running || xAnimation.running

    property alias spriteImageSource: image.source

    onPlayerXChanged: {
        if (Math.abs(playerX - lastPlayerX) > width * 2) {
            targetX = playerX
        } else {
            xAnimation.from = lastPlayerX
            xAnimation.to = playerX
            xAnimation.restart()
        }

        lastPlayerX = playerX
    }

    onPlayerYChanged: {
        if (Math.abs(playerY - lastPlayerY) > height * 2) {
            targetY = playerY
        } else {
            yAnimation.from = lastPlayerY
            yAnimation.to = playerY
            yAnimation.restart()
        }

        lastPlayerY = playerY
    }

    PropertyAnimation on targetX {
        id: xAnimation
        duration: 50
    }


    PropertyAnimation on targetY {
        id: yAnimation
        duration: 50
    }

    property int walkCycle: 0
    Timer {
        interval: 100
        repeat: true
        running: true
        onTriggered: {
            if (walkCycle) {
                walkCycle = 0
            } else {
                walkCycle = 1
            }
        }

    }

    Image {
        id: image
        anchors.fill: parent
        source: "qrc:///sprites/" + playerSprite.spriteBase +  "-" + walkCycle + ".png"
        smooth: false
        visible: true
        opacity: alive
        Behavior on opacity {
            NumberAnimation {
                duration: 500
            }
        }

    }

    Text {
        visible: text !== ""
        anchors.horizontalCenter: image.horizontalCenter
        anchors.top: parent.bottom
        text: playerSprite.name
        color: "white"
        font.strikeout: !playerSprite.alive
        antialiasing: false
        font.pointSize: 16
        font.family: "Perfect DOS VGA 437 Win"
        renderType: Text.NativeRendering
    }
}
