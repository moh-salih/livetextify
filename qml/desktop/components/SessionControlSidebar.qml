import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import LiveTextify
import "../singletons"

Rectangle {
    id: root

    // --- Data Properties ---
    property bool isRecording: false

    // --- Signals ---
    signal toggleRecordingClicked()
    signal endSessionClicked()

    // --- Internal State ---
    property int sessionSeconds: 0

    Timer {
        id: sessionTimer
        interval: 1000
        running: root.isRecording
        repeat: true
        onTriggered: root.sessionSeconds++
    }

    function formatTime(s) {
        let hrs = Math.floor(s / 3600)
        let mins = Math.floor((s % 3600) / 60)
        let secs = s % 60
        return (hrs < 10 ? "0" : "") + hrs + ":" +
               (mins < 10 ? "0" : "") + mins + ":" +
               (secs < 10 ? "0" : "") + secs
    }

    implicitWidth: 320
    color: Theme.surfaceContainerLow

    // Left border line
    Rectangle {
        width: 1; height: parent.height
        anchors.left: parent.left
        color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.2)
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // --- CONTENT AREA ---
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                anchors.centerIn: parent
                width: parent.width - 48
                spacing: 32

                // --- Recording Status ---
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 180
                    radius: 24
                    color: Theme.surfaceContainerHighest
                    clip: true

                    Rectangle {
                        anchors.fill: parent
                        radius: 24
                        color: "transparent"
                        border.color: root.isRecording ? "transparent" : Qt.rgba(255, 183, 77, 0.3)
                        border.width: 1
                        Behavior on border.color { ColorAnimation { duration: 300 } }
                    }

                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: 16

                        Item {
                            Layout.alignment: Qt.AlignHCenter
                            width: 80
                            height: 80

                            Rectangle {
                                anchors.centerIn: parent
                                width: 40; height: 40; radius: 20
                                color: Qt.rgba(Theme.primary.r, Theme.primary.g, Theme.primary.b, 0.2)

                                SequentialAnimation on scale {
                                    loops: Animation.Infinite
                                    running: root.isRecording
                                    NumberAnimation { from: 1.0; to: 2.2; duration: 1500; easing.type: Easing.OutQuad }
                                    NumberAnimation { from: 2.2; to: 1.0; duration: 0 }
                                }
                                SequentialAnimation on opacity {
                                    loops: Animation.Infinite
                                    running: root.isRecording
                                    NumberAnimation { from: 0.8; to: 0.0; duration: 1500; easing.type: Easing.OutQuad }
                                    NumberAnimation { from: 0.0; to: 0.8; duration: 0 }
                                }
                            }

                            Rectangle {
                                anchors.centerIn: parent
                                width: 40; height: 40; radius: 20
                                color: Qt.rgba(Theme.primary.r, Theme.primary.g, Theme.primary.b, 0.4)

                                SequentialAnimation on scale {
                                    loops: Animation.Infinite
                                    running: root.isRecording
                                    PauseAnimation { duration: 400 }
                                    NumberAnimation { from: 1.0; to: 1.6; duration: 1100; easing.type: Easing.OutQuad }
                                    NumberAnimation { from: 1.6; to: 1.0; duration: 0 }
                                }
                                SequentialAnimation on opacity {
                                    loops: Animation.Infinite
                                    running: root.isRecording
                                    PauseAnimation { duration: 400 }
                                    NumberAnimation { from: 1.0; to: 0.0; duration: 1100; easing.type: Easing.OutQuad }
                                    NumberAnimation { from: 0.0; to: 1.0; duration: 0 }
                                }
                            }

                            Rectangle {
                                id: micCenter
                                anchors.centerIn: parent
                                width: 40; height: 40; radius: 20
                                color: root.isRecording ? Theme.primary : Theme.outlineVariant

                                layer.enabled: root.isRecording
                                layer.effect: MultiEffect {
                                    shadowEnabled: true; shadowColor: Theme.primary
                                    shadowOpacity: 0.6; shadowBlur: 20
                                }

                                Text {
                                    anchors.centerIn: parent
                                    text: root.isRecording ? "mic" : "mic_off"
                                    font.family: Fonts.iconFamily
                                    font.pixelSize: 20
                                    color: root.isRecording ? Theme.background : Theme.textOnSurface
                                }

                                Behavior on color { ColorAnimation { duration: 300 } }
                            }
                        }

                        ColumnLayout {
                            Layout.alignment: Qt.AlignHCenter
                            spacing: 4

                            Text {
                                text: root.formatTime(root.sessionSeconds)
                                font.family: Fonts.headlineFamily
                                font.pixelSize: 28
                                font.bold: true
                                color: Theme.textOnSurface
                                Layout.alignment: Qt.AlignHCenter
                                font.features: { "tnum": 1 }
                            }

                            Text {
                                text: root.isRecording ? "RECORDING" : "PAUSED"
                                font.family: Fonts.bodyFamily
                                font.pixelSize: 10
                                font.bold: true
                                font.letterSpacing: 2
                                color: root.isRecording ? Theme.primary : "#ffb74d"
                                Layout.alignment: Qt.AlignHCenter
                            }
                        }
                    }
                }
            }
        }

        // --- PINNED BOTTOM CONTROLS ---
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.2)
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.margins: 24
            Layout.topMargin: 16
            Layout.bottomMargin: 24
            spacing: 12

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 52
                radius: 16

                color: root.isRecording ? (pauseHover.containsMouse ? Qt.rgba(255,255,255, 0.05) : "transparent") : Qt.rgba(Theme.primary.r, Theme.primary.g, Theme.primary.b, 0.1)
                border.color: root.isRecording ? Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.4) : Theme.primary
                border.width: root.isRecording ? 1 : 2

                Behavior on color { ColorAnimation { duration: 150 } }
                Behavior on border.color { ColorAnimation { duration: 150 } }

                RowLayout {
                    anchors.centerIn: parent
                    spacing: 12
                    Text {
                        text: root.isRecording ? "pause" : "play_arrow"
                        font.family: Fonts.iconFamily
                        font.pixelSize: 20
                        color: root.isRecording ? Theme.textOnSurfaceVariant : Theme.primary
                        Behavior on color { ColorAnimation { duration: 150 } }
                    }
                    Text {
                        text: root.isRecording ? "Pause Engine" : "Resume Engine"
                        font.family: Fonts.bodyFamily
                        font.pixelSize: 14
                        font.bold: true
                        color: root.isRecording ? Theme.textOnSurface : Theme.primary
                        Behavior on color { ColorAnimation { duration: 150 } }
                    }
                }

                MouseArea {
                    id: pauseHover
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onPressed: parent.scale = 0.98
                    onReleased: parent.scale = 1.0
                    onClicked: root.toggleRecordingClicked()
                }
                Behavior on scale { NumberAnimation { duration: 100 } }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 52
                radius: 16
                color: Qt.rgba(Theme.errorColor.r, Theme.errorColor.g, Theme.errorColor.b, stopHover.containsMouse ? 0.3 : 0.15)
                border.color: Qt.rgba(Theme.errorColor.r, Theme.errorColor.g, Theme.errorColor.b, 0.3)
                border.width: 1

                Behavior on color { ColorAnimation { duration: 150 } }

                RowLayout {
                    anchors.centerIn: parent
                    spacing: 12
                    Text {
                        text: "stop_circle"
                        font.family: Fonts.iconFamily
                        font.pixelSize: 20
                        color: Theme.errorColor
                    }
                    Text {
                        text: "End Session"
                        font.family: Fonts.bodyFamily
                        font.pixelSize: 14
                        font.bold: true
                        color: Theme.errorColor
                    }
                }

                MouseArea {
                    id: stopHover
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onPressed: parent.scale = 0.98
                    onReleased: parent.scale = 1.0
                    onClicked: root.endSessionClicked()
                }
                Behavior on scale { NumberAnimation { duration: 100 } }
            }
        }
    }
}
