import QtQuick 2.0

Image {
    id: image
    smooth: false
    width: (parent == null || parent.rows === 0) ? 0 : parent.height / (parent.rows + 1)
    height: width
    property QtObject bombData
    property int bombX
    property int bombY
    x: bombX * width;
    y: bombY * height
    source: bombData == null ? "" : "qrc:/sprites/bomb/bomb-" + bombData.state + ".png"



    Connections {
        target: bombData
        onStateChanged: {
            if (bombData.state > 13) {
                image.source = "qrc:/sprites/explosion/flame-" + Math.floor(Math.random() * 5) + ".png"
                explosion.shown = true
                explosion.visible = true
                explosion.opacity = 0
            }
        }
    }

    Behavior on x {
        NumberAnimation {
            easing { type: Easing.OutElastic; amplitude: 1.0; period: 0.9 }
            duration: 500
        }
    }

    Behavior on y {
        NumberAnimation {
            easing { type: Easing.OutElastic; amplitude: 1.0; period: 0.9 }
            duration: 500
        }
    }

    // Flames
    Item {
        anchors.fill: parent
        id: explosion
        opacity: 1
        visible: false

        onOpacityChanged: {
            console.log("FETTE" + shown + opacity)
            if (opacity == 0 && shown) {
                console.log("HOREUNGE")
                parent.destroy()
            }
        }
        property bool shown: false


        Behavior on opacity {
            NumberAnimation {
                //easing { type: Easing.OutElastic; amplitude: 1.0; period: 0.9 }
                duration: 250
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
