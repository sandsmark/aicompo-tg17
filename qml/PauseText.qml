import QtQuick 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Particles 2.0
import org.gathering.ghostly 1.0

Text {
    id: pauseText
    text: "PAUSED"
    color: "white"
    antialiasing: false
    renderType: Text.NativeRendering
    font.pointSize: 40
    opacity: 0.7
    onVisibleChanged: pauseAnimation.restart()
    z: 10
    
    
    Text {
        id: pauseOverlay
        anchors.top: parent.top
        anchors.right: parent.right
        color: "white"
        text: parent.text
        visible: opacity > 0
        font.bold: true
        antialiasing: false
        renderType: Text.NativeRendering
        
        ParallelAnimation {
            id: pauseAnimation
            NumberAnimation {
                target: pauseOverlay
                property: "opacity"
                from: 1
                to: 0
                duration: 200
            }
            NumberAnimation {
                target: pauseOverlay
                property: "font.pointSize"
                from: 40
                to: 200
                duration: 300
            }
        }
    }
}
