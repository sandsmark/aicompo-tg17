import QtQuick 2.0

Rectangle {
    id: startButton
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 10
    anchors.right: parent.horizontalCenter
    anchors.rightMargin: 10
    width: 200
    height: 50
    
    color: "#50000000"
    border.color: "white"
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: MouseArea.PointingHandCursor
        onEntered: {
            startButtonText.color = "black"
            startButton.color = "white"
        }
        onExited: {
            startButtonText.color = "white"
            startButton.color = "#50000000"
        }
        onClicked: {
            if (players.length < 2) {
                return
            }
            startScreen.visible = false
            game.gameStart()
        }
    }
    Text {
        id: startButtonText
        anchors.centerIn: parent
        text: "Start game"
        color: "white"
        font.pointSize: 25
    }
}
