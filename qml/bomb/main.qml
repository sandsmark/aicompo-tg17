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
            pauseText.visible = !pauseText.visible
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
        opacity: 0.7
        visible: false
        z: 10
    }

    Text {
        id: roundsPlayed
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        text: "round: " + game.roundsPlayed + "/5"
        color: "white"
        font.bold: true
        font.pointSize: 40
        z: 10
        opacity: 0.7
    }

    Text {
        id: timeRemaining
        anchors.top: parent.top
        anchors.left: parent.left
        text: game.ticksLeft > 0 ? Math.ceil(game.ticksLeft/4) + "s" : "SUDDEN DEATH"
        color: (game.ticksLeft > 40) ? "white" : "red"
        font.bold: true
        font.pointSize: 40
        opacity: 0.7
        visible: game.ticksLeft != -1
        z: 10
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
        id: aboutText
        anchors.bottom: buildId.top
        anchors.right: parent.right
        color: "white"
        text: "code:martin^lgndr//sound:ferris^yup"
        opacity: 0.5
    }

    Text {
        id: buildId
        anchors.left: aboutText.left
        anchors.bottom: parent.bottom
        color: "white"
        text: "build time: " + game.version()
        opacity: 0.3
    }
}
