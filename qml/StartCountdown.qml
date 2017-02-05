import QtQuick 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Particles 2.0
import org.gathering.ghostly 1.0

Rectangle {
    id: startCountdown


    color: "#a0000000"
    border.color: "white"
    border.width: 4
    
    visible: opacity > 0
    opacity: countdownTimer.running ? 1 : 0
    Behavior on opacity {
        NumberAnimation {
            duration: 500
        }
    }
    
    Rectangle {
        id: countdownFirst
        color: "white"
        opacity: 0
        width: parent.width
        height: width
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }
    }
    
    Rectangle {
        id: countdownSecond
        color: "white"
        opacity: 0
        width: parent.width
        height: width
        anchors {
            verticalCenter: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }
    }
    
    
    Rectangle {
        id: countdownThird
        color: "white"
        opacity: 0
        width: parent.width
        height: width
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
    }
    
    Connections {
        target: GameManager
        onShowCountdown: {
            countdownFirst.opacity = 0
            countdownSecond.opacity = 0
            countdownThird.opacity = 0
            countdownTimer.progress = 0
            countdownTimer.restart()
        }
        onGameRunningChanged: {
            countdownTimer.stop()
        }
    }
    
    Timer {
        id: countdownTimer
        interval: 500
        running: false
        repeat: true
        property int progress: 0
        
        onTriggered: {
            if (progress === 0) {
                countdownFirst.opacity = 1
            } else if (progress === 1) {
                countdownSecond.opacity = 1
            } else {
                countdownThird.opacity = 1
                countdownTimer.stop()
            }
            
            progress++
        }
    }
}
