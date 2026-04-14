import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify

Rectangle {
    id: root

    // Pure UI Properties
    property string sessionTitle: "Active Session"
    property string durationText: "LIVE NOW"

    signal viewStreamClicked()
    signal stopSessionClicked()

    Layout.preferredHeight: 240
    radius: 32
    color: "transparent"

    // Outer gradient border simulation
    Rectangle {
        anchors.fill: parent
        radius: 32
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.rgba(Theme.primary.r, Theme.primary.g, Theme.primary.b, 0.4) }
            GradientStop { position: 1.0; color: Qt.rgba(Theme.secondary.r, Theme.secondary.g, Theme.secondary.b, 0.1) }
        }
        opacity: 0.5
    }

    // Inner Glass Card
    Rectangle {
        anchors.fill: parent
        anchors.margins: 2
        radius: 30
        color: Qt.rgba(Theme.surfaceContainerLowest.r, Theme.surfaceContainerLowest.g, Theme.surfaceContainerLowest.b, 0.8)
        border.color: Qt.rgba(Theme.primary.r, Theme.primary.g, Theme.primary.b, 0.1)

        RowLayout {
            anchors.fill: parent
            anchors.margins: 32
            spacing: 32

            Item {
                Layout.preferredWidth: 100
                Layout.preferredHeight: 100

                Repeater {
                    model: 3
                    Rectangle {
                        anchors.centerIn: parent
                        width: 64
                        height: 64
                        radius: width / 2
                        color: "transparent"
                        border.color: Theme.primary
                        border.width: 2

                        SequentialAnimation on scale {
                            loops: Animation.Infinite
                            PauseAnimation { duration: index * 600 }
                            NumberAnimation { from: 1.0; to: 2.5; duration: 2000; easing.type: Easing.OutQuad }
                        }
                        SequentialAnimation on opacity {
                            loops: Animation.Infinite
                            PauseAnimation { duration: index * 600 }
                            NumberAnimation { from: 0.6; to: 0.0; duration: 2000; easing.type: Easing.OutQuad }
                        }
                    }
                }

                Rectangle {
                    anchors.centerIn: parent
                    width: 64
                    height: 64
                    radius: 32
                    gradient: Theme.primaryGradient

                    Text {
                        anchors.centerIn: parent
                        text: "mic"
                        font.family: Fonts.iconFamily
                        font.pixelSize: 32
                        color: Theme.background
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 8

                RowLayout {
                    spacing: 12

                    Rectangle {
                        color: Qt.rgba(255, 113, 108, 0.1)
                        border.color: Qt.rgba(255, 113, 108, 0.3)
                        radius: 12
                        Layout.preferredHeight: 24
                        Layout.preferredWidth: 80

                        RowLayout {
                            anchors.centerIn: parent
                            spacing: 6
                            Rectangle {
                                width: 6; height: 6; radius: 3; color: "#ff716c"
                                SequentialAnimation on opacity {
                                    loops: Animation.Infinite
                                    NumberAnimation { to: 0.2; duration: 800 }
                                    NumberAnimation { to: 1.0; duration: 800 }
                                }
                            }
                            Text {
                                text: "ACTIVE"
                                font.family: Fonts.bodyFamily
                                font.pixelSize: Fonts.micro
                                font.bold: true
                                color: "#ff716c"
                            }
                        }
                    }

                    Text {
                        text: root.durationText
                        font.family: Fonts.bodyFamily
                        font.pixelSize: Fonts.tiny
                        font.bold: true
                        font.letterSpacing: 1
                        color: Theme.textOnSurfaceVariant
                        opacity: 0.6
                    }
                }

                Text {
                    text: root.sessionTitle
                    font.family: Fonts.headlineFamily
                    font.pixelSize: Fonts.displaySmall
                    font.bold: true
                    color: Theme.textOnSurface

                    // FIX: Force the text to shrink below its implicit width to trigger elide
                    Layout.fillWidth: true
                    Layout.preferredWidth: 0
                    elide: Text.ElideRight
                }

                Text {
                    text: "Neural transcription active. Real-time RAG processing enabled."
                    font.family: Fonts.bodyFamily
                    font.pixelSize: Fonts.large
                    color: Theme.textOnSurfaceVariant
                    opacity: 0.8
                    Layout.fillWidth: true
                    Layout.preferredWidth: 0
                    elide: Text.ElideRight
                }
            }

            RowLayout {
                spacing: 16

                Rectangle {
                    Layout.preferredWidth: 160
                    Layout.preferredHeight: 56
                    radius: 16
                    color: "transparent"
                    border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.5)

                    Text {
                        anchors.centerIn: parent
                        text: "View Stream"
                        font.family: Fonts.bodyFamily
                        font.pixelSize: Fonts.normal
                        font.bold: true
                        color: Theme.textOnSurface
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onEntered: parent.color = Qt.rgba(255, 255, 255, 0.05)
                        onExited: parent.color = "transparent"
                        onClicked: root.viewStreamClicked()
                    }
                }

                Rectangle {
                    Layout.preferredWidth: 160
                    Layout.preferredHeight: 56
                    radius: 16
                    color: "#ff716c"

                    Text {
                        anchors.centerIn: parent
                        text: "Stop Session"
                        font.family: Fonts.bodyFamily
                        font.pixelSize: Fonts.normal
                        font.bold: true
                        color: Theme.textOnSurface
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onPressed: parent.scale = 0.95
                        onReleased: parent.scale = 1.0
                        onClicked: root.stopSessionClicked()
                    }
                    Behavior on scale { NumberAnimation { duration: 100 } }
                }
            }
        }
    }
}
