import QtQuick 2.0

Rectangle {
    color: "black"

    signal userMove(string direction)

    Keys.onDownPressed: userMove("DOWN")
    Keys.onUpPressed: userMove("UP")
    Keys.onRightPressed: userMove("RIGHT")
    Keys.onLeftPressed: userMove("LEFT")
    Keys.onBackPressed: userMove("BOMB")
    Keys.onEscapePressed: {
        game.stopGame();
        startScreen.opacity = 1
    }
    Keys.onSpacePressed: game.togglePause()

    Keys.onPressed: {
        if (event.key === Qt.Key_F5) {
            game.endRound();
            return true;
        } else if (event.key == Qt.Key_Backspace) {
            userMove("BOMB")
            return true;
        }

        return false;
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
        text: "version: " + game.version()
    }
}
