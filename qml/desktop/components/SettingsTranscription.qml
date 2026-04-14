import QtQuick
import QtQuick.Layouts
import LiveTextify

SettingsSection {
    id: root
    title: "Transcription"
    themeColor: Theme.tertiary

    // Dependencies
    property string language: "English (US)"
    property real vadSensitivity: 0.75

    SettingsRow {
        Layout.fillWidth: true
        title: "Neural Language"
        description: "Primary detection model language."

        RowLayout {
            spacing: 8

            Rectangle {
                Layout.preferredWidth: 140
                Layout.preferredHeight: 40
                radius: 8
                color: Theme.surfaceContainerLowest
                border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.3)

                RowLayout {
                    anchors.centerIn: parent
                    spacing: 12
                    Rectangle { width: 8; height: 8; radius: 4; color: Theme.primary }
                    Text { text: root.language; font.family: Fonts.bodyFamily; font.pixelSize: Fonts.normal; color: Theme.textOnSurface }
                }
            }

            Rectangle {
                Layout.preferredWidth: 40
                Layout.preferredHeight: 40
                radius: 8
                color: "transparent"
                Text {
                    anchors.centerIn: parent
                    text: "edit"
                    font.family: Fonts.iconFamily
                    font.pixelSize: Fonts.title
                    color: Theme.primary
                }
            }
        }
    }

    ColumnLayout {
        Layout.fillWidth: true
        spacing: 16

        RowLayout {
            Layout.fillWidth: true
            ColumnLayout {
                spacing: 4
                Text {
                    text: "VAD Sensitivity"
                    font.family: Fonts.headlineFamily
                    font.pixelSize: Fonts.title
                    font.bold: true
                    color: Theme.textOnSurface
                }
                Text {
                    text: "Voice Activity Detection threshold."
                    font.family: Fonts.bodyFamily
                    font.pixelSize: Fonts.normal
                    color: Theme.textOnSurfaceVariant
                }
            }
            Item { Layout.fillWidth: true }
            Text {
                text: Math.round(root.vadSensitivity * 100) + "%"
                font.family: Fonts.bodyFamily
                font.pixelSize: Fonts.normal
                font.bold: true
                color: Theme.primary
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 8
            radius: 4
            color: Theme.surfaceContainerLowest

            Rectangle {
                width: parent.width * root.vadSensitivity
                height: parent.height
                radius: 4
                gradient: Theme.primaryGradient
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onPositionChanged: (mouse) => {
                    if (pressed) root.vadSensitivity = Math.max(0.0, Math.min(1.0, mouse.x / width))
                }
                onClicked: (mouse) => {
                    root.vadSensitivity = Math.max(0.0, Math.min(1.0, mouse.x / width))
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Text { text: "WHISPER"; font.family: Fonts.bodyFamily; font.pixelSize: Fonts.tiny; font.bold: true; font.letterSpacing: 2; color: Theme.textOnSurfaceVariant }
            Item { Layout.fillWidth: true }
            Text { text: "NORMAL"; font.family: Fonts.bodyFamily; font.pixelSize: Fonts.tiny; font.bold: true; font.letterSpacing: 2; color: Theme.textOnSurfaceVariant; Layout.alignment: Qt.AlignHCenter }
            Item { Layout.fillWidth: true }
            Text { text: "AGGRESSIVE"; font.family: Fonts.bodyFamily; font.pixelSize: Fonts.tiny; font.bold: true; font.letterSpacing: 2; color: Theme.textOnSurfaceVariant }
        }
    }
}
