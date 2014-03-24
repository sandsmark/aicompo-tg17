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
    source: bombData == null ? "" : "qrc:/sprites/bomb/bomb-" + (bombData.state + 8) + ".png"


    Behavior on opacity {
        NumberAnimation {
            duration: 250
        }
    }

    Connections {
        target: bombData
        onStateChanged: {
            if (bombData.state > 13) {
                image.opacity = 0
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
}
