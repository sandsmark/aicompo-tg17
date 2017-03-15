import QtQuick 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Particles 2.0
import org.gathering.ghostly 1.0

Item {
    id: playerNameList
    Rectangle {
        anchors.fill: playerNameColumn
        color: "#c0000000"
    }

    property int totalPoints: 0
    Connections {
        target: GameManager
        onGameStarted: totalPoints = 0
    }

    Column {
        id: playerNameColumn
        spacing: 5

        Repeater {
            model: GameManager.players
            delegate: Row {
                spacing: 10
                Image {
                    id: playerIcon
                    source: "qrc:///sprites/players/player" + modelData.id + "-" + (modelData.alive ?  modelData.direction + "-0" : "dead") + ".png"
                    height: playerName.height
                    width: height
                    smooth: false
                }
                property int previousPoints: 0
                Connections {
                    target: modelData
                    onPointsChanged:{
                        playerNameList.totalPoints += (modelData.points - previousPoints)
                        previousPoints = modelData.points
                    }
                }

                Item {
                    width: 100
                    height: playerName.height
                    property real relativePoints: playerNameList.totalPoints > 0 ? (modelData.points > 0 ? (modelData.points / playerNameList.totalPoints) : 0) : 1
                    opacity: relativePoints
                    Rectangle {
                        color: playerColors[modelData.id]
                        width: parent.relativePoints * parent.width
                        height: playerName.height
                        Behavior on width { NumberAnimation { duration: 100; } }
                    }

                    Text {
                        anchors.centerIn: parent
                        color: "white"
                        style: Text.Outline
                        styleColor: "black"
                        text: modelData.points
                    }
                }

                Text {
                    id: playerName
                    text: modelData.name + " (" + modelData.wins + ")"
                    color: "white"
                    style: Text.Outline
                    styleColor: "black"
                    font.strikeout: !modelData.alive
                }
            }
        }
    }
}
