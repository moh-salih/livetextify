import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify
import "../singletons"

RowLayout {
    id: root

    // --- Data Properties ---
    property bool hasModels: true

    // --- Action Signals ---
    signal newTranscriptionClicked()
    signal libraryClicked()

    ColumnLayout {
        Layout.fillWidth: true
        spacing: 8

        RowLayout {
            spacing: 8
            Text {
                text: "Hello,"
                font.family: Fonts.headlineFamily
                font.pixelSize: Fonts.display
                font.bold: true
                color: Theme.textOnSurface
            }
            Text {
                text: "User"
                font.family: Fonts.headlineFamily
                font.pixelSize: Fonts.display
                font.bold: true
                color: Theme.primary
            }
        }
        Text {
            text: root.hasModels
                ? "Ready to transform your ideas into text. Start a new transcription whenever you're ready."
                : "You need a transcription model to begin. Visit the Model Library to download one and get started."
            font.family: Fonts.bodyFamily
            font.pixelSize: Fonts.large
            color: Theme.textOnSurfaceVariant
            Layout.maximumWidth: 450
            wrapMode: Text.WordWrap
        }
    }

    Rectangle {
        Layout.preferredWidth: 260
        Layout.preferredHeight: 64
        radius: 16

        // Dynamically style based on state
        color: root.hasModels ? "transparent" : Theme.surfaceContainerHighest
        gradient: root.hasModels ? Theme.primaryGradient : null
        border.color: root.hasModels ? "transparent" : Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.4)
        border.width: root.hasModels ? 0 : 1

        RowLayout {
            anchors.centerIn: parent
            spacing: 12

            Text {
                text: root.hasModels ? "auto_awesome" : "layers"
                font.family: Fonts.iconFamily
                font.pixelSize: Fonts.largeTitle
                color: root.hasModels ? Theme.background : Theme.primary
            }
            Text {
                text: root.hasModels ? "New Transcription" : "Visit Model Library"
                font.family: Fonts.headlineFamily
                font.pixelSize: Fonts.title
                font.bold: true
                color: root.hasModels ? Theme.background : Theme.primary
            }
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            hoverEnabled: true

            onEntered: {
                if (!root.hasModels) parent.border.color = Theme.primary
            }
            onExited: {
                if (!root.hasModels) parent.border.color = Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.4)
            }
            onPressed: parent.scale = 0.98
            onReleased: parent.scale = 1.0

            onClicked: {
                if (root.hasModels) {
                    root.newTranscriptionClicked()
                } else {
                    root.libraryClicked()
                }
            }
        }

        Behavior on scale { NumberAnimation { duration: 100 } }
        Behavior on border.color { ColorAnimation { duration: 150 } }
    }
}
