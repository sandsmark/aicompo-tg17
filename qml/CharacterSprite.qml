import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: characterSprite

    property string spriteBase
    property bool alive
    property string name: ""

    height: charactersContainer.spriteSize
    width: height

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
        source: "qrc:///sprites/" + characterSprite.spriteBase +  "-" + walkCycle + ".png"
        smooth: false
        visible: true
        opacity: alive
        Behavior on opacity {
            NumberAnimation {
                duration: 500
            }
        }
    }

    Rectangle {
        anchors.fill: title
        opacity: 0.5
        color: "black"
    }

    Text {
        id: title
        anchors {
            horizontalCenter: image.horizontalCenter
            top: image.bottom
        }

        text: characterSprite.name
        visible: text !== ""
        color: "white"
        font.strikeout: !characterSprite.alive
        antialiasing: false
        font.pointSize: 16
        font.family: "Perfect DOS VGA 437 Win"
        renderType: Text.NativeRendering
        style: Text.Outline
        styleColor: "black"
    }
}
