import QtQuick 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Particles 2.0
import org.gathering.ghostly 1.0

Rectangle {
    id: main

    color: "black"

    height: 1200
    width: 1600

    property int scaleSize: (width < height) ? width : height
    property bool effectsEnabled: true

    property var playerColors: [
        "#ff400f", // red
        "#f601ff", // pink
        "#01ffde", // cyan
        "#f0ff40", // yellow
        "#400fff", // blue
        "#40ff0f", // green
        "#ffa201", // brown
        "#a801ff"  // purple
    ]

    signal userMove(string direction)

    focus: true
    Keys.onUpPressed: userMove("up")
    Keys.onDownPressed: userMove("down")
    Keys.onRightPressed: userMove("right")
    Keys.onLeftPressed: userMove("left")
    Keys.onEscapePressed: {
        GameManager.stopGame();
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_F5) {
            GameManager.endRound();
            event.accepted = true
        } else if (event.key === Qt.Key_P) {
            GameManager.togglePause()
            event.accepted = true
        } else {
            event.accepted = false
        }
    }

    Rectangle {
        id: gameArea
        anchors.fill: parent
        color: "black"

        Text {
            id: timerText
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }

            font.pixelSize: 64
            text: GameManager.timeElapsed
            color: "white"
            antialiasing: false
            renderType: Text.NativeRendering
            visible: GameManager.gameRunning
        }

        Rectangle {
            id: pelletBar
            anchors {
                top: timerText.bottom
                horizontalCenter: parent.horizontalCenter
            }
            height: 20
            width: 500
            color: "transparent"
            border.color: "white"
            border.width: 4
            visible: GameManager.gameRunning

            Rectangle {
                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                }
                width: parent.width * Math.max(Map.pelletsLeft, 1) / Map.totalPellets
            }
        }


        PauseText {
            id: pauseText
            anchors {
                right: parent.right
                top: parent.top
            }
            visible: GameManager.gameRunning && !GameManager.roundRunning && !startCountdown.visible
        }

        Text {
            id: roundsPlayed
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            text: "round: " + (GameManager.roundsPlayed + 1) + "/" + GameManager.maxRounds
            color: "white"
            font.bold: true
            font.pointSize: 40
            antialiasing: false
            renderType: Text.NativeRendering
        }

        Item {
            id: playingField
            anchors {
                top: pelletBar.bottom
                bottom: roundsPlayed.top
                left: parent.left
                right: parent.right
            }

            opacity: startScreen.visible ? 0.1 : 1
            Behavior on opacity {
                NumberAnimation { duration: 500 }
            }

            GameGrid {
                id: gameGrid
                anchors.centerIn: parent
            }
        }


        Particles {
            id: particles

            anchors.fill: parent
        }

        // Create player sprites
        CharactersContainer {
            id: charactersContainer

            x: gameGrid.x + playingField.x
            y: gameGrid.y + playingField.y
            spriteSize: gameGrid.tileSize
        }

        // List of names of players
        PlayerNameList {
            id: playerNameList
            anchors {
                top: parent.top
                left: parent.left
            }
        }


        StartCountdown {
            id: startCountdown
            anchors.centerIn: parent
            height: parent.height / 2
            width: height / 3
        }


        EndScreen {
            opacity: (GameManager.roundsPlayed >= GameManager.maxRounds) ? 1 : 0
            anchors.fill: parent
            objectName: "endScreen"
        }

        StartScreen {
            id: startScreen
            anchors.fill: parent
            opacity: GameManager.gameRunning ? 0 : 1
        }

        Text {
            id: aboutText
            anchors.bottom: buildId.top
            anchors.right: parent.right
            anchors.margins: 10
            color: "white"
            text: "code:martin^reMarkable"
            font.pointSize: 10
            antialiasing: false
            renderType: Text.NativeRendering
            opacity: 0.5
        }

        Text {
            id: buildId
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10
            color: "white"
            text: GameManager.version()
            font.pointSize: 10
            antialiasing: false
            renderType: Text.NativeRendering
            opacity: 0.3
        }
    }

//    FastBlur {
//        id: blurFilter
//        anchors.fill: gameArea
//        source: gameArea
//        radius: 8
//    }
//    BrightnessContrast {
//        id: gameFilter
//        source: blurFilter
//        anchors.fill: blurFilter
//        opacity: 1
////        NumberAnimation on opacity {
////            running: false
////            id: blurAnimation
////            duration: 250
////            from: 0.7
////            to: 0
////        }
//        brightness: 0.9
//        contrast: 1
//    }

    Loader {
        id: retroShaderLoader
        anchors.fill: gameArea
    }

    Component {
        id: retroShaderComponent

        Item {
            OldSchoolShader {
                id: retroShader
                anchors.fill: parent
                source: ShaderEffectSource {
                    id: mainSource
                    sourceItem: gameArea
                    width: gameArea.width
                    height: gameArea.height
                    hideSource: true
                }
                burninSource: burninTexture
            }

            // Need to have these parented outside the main shader above, to avoid crashings
            ShaderEffectSource {
                id: burninTexture
                hideSource: true
                sourceItem: burninShader
                recursive: true
            }

            ShaderEffect {
                id: burninShader
                property variant txt_source: retroShader.source
                property variant blurredSource: burninTexture
                property real fps: 60
                property real blurCoefficient: 1.0 / (fps * 0.64)
                width: gameArea.width
                height: gameArea.height
                blending: false

                fragmentShader:
                    "#ifdef GL_ES
                    precision mediump float;
                #endif\n" +

                "uniform lowp float qt_Opacity;" +
                "uniform lowp sampler2D txt_source;" +

                "varying highp vec2 qt_TexCoord0;

                 uniform lowp sampler2D blurredSource;
                 uniform highp float blurCoefficient;" +

                "float rgb2grey(vec3 v){
                    return dot(v, vec3(0.21, 0.72, 0.04));
                }" +

                "void main() {" +
                "vec2 coords = qt_TexCoord0;" +
                "vec3 origColor = texture2D(txt_source, coords).rgb;" +
                "vec3 blur_color = texture2D(blurredSource, coords).rgb - vec3(blurCoefficient);" +
                "vec3 color = min(origColor + blur_color, max(origColor, blur_color));" +

                "gl_FragColor = vec4(color, rgb2grey(color - origColor));" +
                "}"
            }
        }
    }

//    ShaderEffect {
//        id: gameFilter
//        //        visible: false
//        //        source: gameArea
//        property variant source: ShaderEffectSource {
//            sourceItem: gameArea
//            width: gameArea.width
//            height: gameArea.height
//        }

//        anchors.fill: gameArea
//        property real targetWidth: width
//        property real targetHeight: height
//        property real granularity: GameManager.gameRunning ? 1 : 10
//        visible: granularity > 1
//        Behavior on granularity { NumberAnimation { easing.type: "InExpo"; duration: 1000 } }


//        vertexShader: "
//            uniform highp mat4 qt_Matrix;
//            attribute highp vec4 qt_Vertex;
//            attribute highp vec2 qt_MultiTexCoord0;
//            varying highp vec2 qt_TexCoord0;
//            void main() {
//                qt_TexCoord0 = qt_MultiTexCoord0;
//                gl_Position = qt_Matrix * qt_Vertex;
//            }"

//        fragmentShader: "
//            uniform float granularity;
//            uniform float targetWidth;
//            uniform float targetHeight;

//            uniform sampler2D source;
//            uniform float qt_Opacity;
//            varying vec2 qt_TexCoord0;

//            void main()
//            {
//                vec2 uv = qt_TexCoord0.xy;
//                vec2 tc = qt_TexCoord0;
//                if (granularity > 0.0) {
//                    float dx = granularity / targetWidth;
//                    float dy = granularity / targetHeight;
//                    tc = vec2(dx*(floor(uv.x/dx) + 0.5),
//                              dy*(floor(uv.y/dy) + 0.5));
//                }
//                gl_FragColor = qt_Opacity * texture2D(source, tc);
//            }"
//    }
}
