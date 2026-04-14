import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify

RowLayout {
    id: root
    signal newTranscriptionClicked()

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
            text: "Ready to transform your ideas into text. Start a new transcription whenever you're ready"
            font.family: Fonts.bodyFamily
            font.pixelSize: Fonts.large
            color: Theme.textOnSurfaceVariant
            Layout.maximumWidth: 450
            wrapMode: Text.WordWrap
        }
    }

    Rectangle {
        Layout.preferredWidth: 240
        Layout.preferredHeight: 64
        radius: 16
        gradient: Theme.primaryGradient

        RowLayout {
            anchors.centerIn: parent
            spacing: 12

            Text {
                text: "auto_awesome"
                font.family: Fonts.iconFamily
                font.pixelSize: Fonts.largeTitle
                color: Theme.background
            }
            Text {
                text: "New Transcription"
                font.family: Fonts.headlineFamily
                font.pixelSize: Fonts.title
                font.bold: true
                color: Theme.background
            }
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onPressed: parent.scale = 0.98
            onReleased: parent.scale = 1.0
            onClicked: root.newTranscriptionClicked()
        }

        Behavior on scale {
            NumberAnimation {
                duration: 100
            }
        }
    }
}
