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

    property int totalScore: 0
    Connections {
        target: GameManager
        onGameStarted: totalScore = 0
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
                property int previousScore: 0
                Connections {
                    target: modelData
                    onScoreChanged:{
                        playerNameList.totalScore += (modelData.score - previousScore)
                        previousScore = modelData.score
                    }
                }

                Item {
                    width: 100
                    height: playerName.height
                    property real relativeScore: playerNameList.totalScore > 0 ? (modelData.score > 0 ? (modelData.score / playerNameList.totalScore) : 0) : 1
                    opacity: relativeScore
                    Rectangle {
                        color: playerColors[modelData.id]
                        width: parent.relativeScore * parent.width
                        height: playerName.height
                        Behavior on width { NumberAnimation { duration: 100; } }
                    }

                    Text {
                        anchors.centerIn: parent
                        color: "white"
                        style: Text.Outline
                        styleColor: "black"
                        text: modelData.score
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
