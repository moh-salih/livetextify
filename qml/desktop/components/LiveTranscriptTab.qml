import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify
import "../singletons"

Rectangle {
    id: root

    // --- Pure UI Properties ---
    property var session: null
    property bool isRecording: false

    // --- Pure UI Signals ---
    signal copyTextRequested(string text)

    Layout.fillWidth: true
    Layout.fillHeight: true
    color: "transparent" // Let parent page background show

    ScrollView {
        id: transcriptScroll
        anchors.fill: parent
        clip: true

        bottomPadding: 80
        contentWidth: availableWidth

        ColumnLayout {
            width: Math.min(800, transcriptScroll.availableWidth - 64)
            anchors.horizontalCenter: parent.horizontalCenter
            y: 32
            spacing: 24

            onHeightChanged: {
                transcriptScroll.ScrollBar.vertical.position = 1.0
            }

            // Glowing Text Card Background
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: contentCol.implicitHeight + 64
                radius: 24
                color: Qt.rgba(Theme.surfaceContainerLow.r, Theme.surfaceContainerLow.g, Theme.surfaceContainerLow.b, 0.3)
                border.color: Qt.rgba(Theme.primary.r, Theme.primary.g, Theme.primary.b, 0.15) // inner-glow-cyan

                // Inner soft cyan shadow simulation
                Rectangle {
                    anchors.fill: parent
                    radius: 24
                    color: "transparent"
                    border.width: 2
                    border.color: Qt.rgba(Theme.primary.r, Theme.primary.g, Theme.primary.b, 0.05)
                }

                ColumnLayout {
                    id: contentCol
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 32
                    spacing: 24

                    Text {
                        text: root.session && root.session.transcription !== "" ? root.session.transcription : "Welcome everyone. System analysis active, awaiting live audio stream..."
                        font.family: Fonts.headlineFamily
                        font.pixelSize: Fonts.heading
                        color: Theme.textOnSurface
                        opacity: root.session && root.session.transcription !== "" ? 0.9 : 0.5
                        font.weight: Font.Light
                        font.italic: !root.session || root.session.transcription === ""
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                        lineHeight: 1.6
                    }

                    // Bouncing dots indicating active listening
                    RowLayout {
                        visible: root.isRecording
                        spacing: 8
                        Layout.topMargin: 16

                        BouncingDots {}

                        Text {
                            text: "Live Stream Processing..."
                            font.family: Fonts.bodyFamily
                            font.pixelSize: Fonts.small
                            font.italic: true
                            font.bold: true
                            color: Theme.primary
                            opacity: 0.8
                            font.letterSpacing: 1
                        }
                    }
                }
            }
        }
    }

    TextEdit {
        id: clipboardHelper
        visible: false
    }
}
