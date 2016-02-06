import QtQuick 2.0

Rectangle {
    id: checkbox
    height: 20
    width: 20
    border.color: "white"
    color: "transparent"

    signal clicked
    property bool checked: false

    Behavior on opacity {
        NumberAnimation {
            duration: 250
        }
    }
    
    Rectangle {
        id: checkMark
        anchors.fill: parent
        anchors.margins: 3
        border.color: "white"
        opacity: checkbox.checked ? (mouseArea.containsMouse ? 1.0 : 0.7) : (mouseArea.containsMouse ? 0.3 : 0.0)
        border.width: 1
        color: checkbox.checked ? "white" : "black"
    }
    
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        cursorShape: MouseArea.PointingHandCursor
        hoverEnabled: true
        
        onClicked: checkbox.clicked()
    }
}
