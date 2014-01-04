import QtQuick 2.0

Image {
    id: image
    smooth: false
    width: 64
    height: 64
    property QtObject bombData
    property int bombX: bombData == null ? 0 : bombData.position.x * 64;
    property int bombY: bombData == null ? 0 : bombData.position.y * 64;
    x: bombX
    y: bombY
    source: bombData == null ? "" : "qrc:/sprites/bomb/bomb-" + bombData.state + ".png"

    Connections {
        target: bombData
        onStateChanged: {
            if (bombData.state > 13) {
                image.source = "qrc:/sprites/explosion/flame-" + Math.floor(Math.random() * 5) + ".png"
                explosion.opacity = 1
            }
        }
    }

    Item {
        anchors.fill: parent
        id: explosion
        opacity: 0


        Behavior on opacity {
            NumberAnimation {
                //easing { type: Easing.OutElastic; amplitude: 1.0; period: 0.9 }
                duration: 300
            }
        }

        Image {
            x: 0
            y: -64
            width: 64; height: 64; smooth: false; source: "qrc:/sprites/explosion/flame-" + Math.floor(Math.random() * 5) + ".png"
        }
        Image {
            x: 0
            y: -128
            width: 64; height: 64; smooth: false; source: "qrc:/sprites/explosion/flame-" + Math.floor(Math.random() * 5) + ".png"
        }
        Image {
            x: 0
            y: 64
            width: 64; height: 64; smooth: false; source: "qrc:/sprites/explosion/flame-" + Math.floor(Math.random() * 5) + ".png"
        }
        Image {
            x: 0
            y: 128
            width: 64; height: 64; smooth: false; source: "qrc:/sprites/explosion/flame-" + Math.floor(Math.random() * 5) + ".png"
        }
        Image {
            x: -64
            y: 0
            width: 64; height: 64; smooth: false; source: "qrc:/sprites/explosion/flame-" + Math.floor(Math.random() * 5) + ".png"
        }

        Image {
            x: -128
            y: 0
            width: 64; height: 64; smooth: false; source: "qrc:/sprites/explosion/flame-" + Math.floor(Math.random() * 5) + ".png"
        }

        Image {
            x: 64
            y: 0
            width: 64; height: 64; smooth: false; source: "qrc:/sprites/explosion/flame-" + Math.floor(Math.random() * 5) + ".png"
        }

        Image {
            x: 128
            y: 0
            width: 64; height: 64; smooth: false; source: "qrc:/sprites/explosion/flame-" + Math.floor(Math.random() * 5) + ".png"
        }
    }
}
