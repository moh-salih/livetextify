import QtQuick
import QtQuick.Layouts
import LiveTextify

Rectangle {
    id: root
    height: 88
    // Gives a frosted glass look docked at the bottom
    color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, 0.9)

    signal discardClicked()
    signal syncClicked()

    // Top border line
    Rectangle {
        width: parent.width
        height: 1
        anchors.top: parent.top
        color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.4)
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 40
        anchors.rightMargin: 40
        spacing: 24

        Item { Layout.fillWidth: true }

        Text {
            text: "Discard Changes"
            font.family: Fonts.headlineFamily
            font.pixelSize: Fonts.normal
            font.bold: true
            color: Theme.textOnSurfaceVariant

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: root.discardClicked()
            }
        }

        Rectangle {
            Layout.preferredWidth: 200
            Layout.preferredHeight: 48
            radius: 12
            gradient: Theme.primaryGradient

            Text {
                anchors.centerIn: parent
                text: "Sync Preferences"
                font.family: Fonts.headlineFamily
                font.pixelSize: Fonts.normal
                font.bold: true
                color: Theme.background
            }
            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onPressed: parent.scale = 0.95
                onReleased: parent.scale = 1.0
                onClicked: root.syncClicked()
            }
            Behavior on scale { NumberAnimation { duration: 100 } }
        }
    }
}
