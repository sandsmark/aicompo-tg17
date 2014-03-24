import QtQuick 2.0

Rectangle {
    color: "black"

    height: 650
    width: 850

    signal userMove(string direction)

    Keys.onDownPressed: userMove("DOWN")
    Keys.onUpPressed: userMove("UP")
    Keys.onRightPressed: userMove("RIGHT")
    Keys.onLeftPressed: userMove("LEFT")
    Keys.onSpacePressed: userMove("BOMB")
    Keys.onEscapePressed: {
        game.stopGame();
        startScreen.opacity = 1
    }
    Keys.onPressed: {
        if (event.key === Qt.Key_F5) {
            game.endRound();
            return true;
        } else if (event.key === Qt.Key_P) {
            game.togglePause()
            pauseText.opacity = 1 - pauseText.opacity
            return true;
        }

        return false;
    }

    Text {
        id: pauseText
        anchors.top: parent.top
        anchors.right: parent.right
        text: "PAUSED"
        color: "white"
        font.bold: true
        font.pointSize: 40
        opacity: 0
    }

    Text {
        id: roundsPlayed
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        text: "round: " + game.roundsPlayed + "/5"
        color: "white"
        font.bold: true
        font.pointSize: 40
    }

    Text {
        id: timeRemaining
        anchors.top: parent.top
        anchors.left: parent.left
        text: game.ticksLeft > 0 ? Math.ceil(game.ticksLeft/4) + "s" : "SUDDEN DEATH"
        color: (game.ticksLeft > 40) ? "white" : "red"
        font.bold: true
        font.pointSize: 40
        opacity: 1
        visible: game.ticksLeft != -1
    }


    focus: true

    // Map background grid
    PlayingField {
        id: playingField
    }

    // List of player sprites
    Repeater {
        model: players
        delegate: PlayerSprite {
            playerId: model.modelData.id
            x: playingField.x + model.modelData.position.x * height
            y: playingField.y + model.modelData.position.y * height
            height: playingField.height / (playingField.rows + 1)
            width: height
        }
    }

    EndScreen {
        opacity: 0
        objectName: "endScreen"
    }

    StartScreen {
        id: startScreen
    }

    Text {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "white"
        text: "build time: " + game.version()
    }
}
