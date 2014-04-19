import QtQuick 2.0

Rectangle {
    signal clicked
    property string text
    color: "#50000000"
    border.color: "white"
    enabled: visible
    visible: opacity > 0
    MouseArea {
        enabled: parent.enabled
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: MouseArea.PointingHandCursor
        onEntered: {
            startButtonText.color = "black"
            parent.color = "white"
        }
        onExited: {
            startButtonText.color = "white"
            parent.color = "#50000000"
        }
        onClicked: parent.clicked()
    }
    Text {
        id: startButtonText
        anchors.centerIn: parent
        text: parent.text
        color: "white"
        font.pointSize: 20
    }
}
