import QtQuick 2.0

Rectangle {
    anchors.fill: parent
    width: 100
    height: 62

    Behavior on opacity {
        NumberAnimation {
            easing { type: Easing.OutElastic; amplitude: 1.0; period: 0.9 }
            duration: 5000
        }
    }

    color: "#a0000000"
    Text {
        anchors.centerIn: parent
        font.pointSize: 25
        font.bold: true
        text: "Game Over"
        color: "white"
        style: Text.Outline
    }
}
