import QtQuick 2.0

Rectangle {
    id: button
    signal clicked
    property string text
    property bool active: true
    property int fontSize: 20

    color: "#7f000000"
    border.color: "white"
    enabled: visible && active
    visible: opacity > 0
    MouseArea {
        id: mouseArea
        enabled: parent.enabled
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: MouseArea.PointingHandCursor
        onEntered: {
            parent.color = "white"
        }
        onExited: {
            parent.color = "#7f000000"
        }
        onClicked: parent.clicked()
    }
    Text {
        id: startButtonText
        anchors.centerIn: parent
        text: parent.text
        color: button.active ? (mouseArea.containsMouse ? "black" : "white") : "gray"
        font.pointSize: button.fontSize
    }
}
