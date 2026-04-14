import QtQuick
import QtQuick.Layouts
import LiveTextify

SettingsSection {
    id: root
    title: "Storage & Privacy"
    themeColor: "#ff716c" // Theme.error

    // Dependencies
    property string localBufferGB: "12.4"
    property string totalBufferGB: "50"
    property real bufferPercentage: 0.248

    signal purgeClicked()

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 120
        color: Theme.surfaceContainerLowest
        radius: 16
        border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.2)

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 24

            RowLayout {
                Layout.fillWidth: true
                ColumnLayout {
                    spacing: 4
                    Text {
                        text: "LOCAL BUFFER"
                        font.family: Fonts.bodyFamily
                        font.pixelSize: Fonts.small
                        font.bold: true
                        font.letterSpacing: 2
                        color: Theme.textOnSurfaceVariant
                    }
                    Text {
                        text: root.localBufferGB + " <font size='4' color='" + Qt.rgba(255, 255, 255, 0.4) + "'>GB</font>"
                        font.family: Fonts.headlineFamily
                        font.pixelSize: 32
                        font.bold: true
                        color: Theme.textOnSurface
                        textFormat: Text.RichText
                    }
                }
                Item { Layout.fillWidth: true }
                ColumnLayout {
                    spacing: 4
                    Text {
                        text: "Used of " + root.totalBufferGB + "GB"
                        font.family: Fonts.bodyFamily
                        font.pixelSize: Fonts.small
                        color: Theme.textOnSurfaceVariant
                        Layout.alignment: Qt.AlignRight
                    }
                    Text {
                        text: Math.round(root.bufferPercentage * 1000) / 10 + "%"
                        font.family: Fonts.bodyFamily
                        font.pixelSize: Fonts.normal
                        font.bold: true
                        color: Theme.tertiary
                        Layout.alignment: Qt.AlignRight
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 6
                radius: 3
                color: Theme.surfaceContainerHighest
                Rectangle {
                    width: parent.width * root.bufferPercentage
                    height: parent.height
                    radius: 3
                    color: Theme.tertiary
                }
            }
        }
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: 16
        Layout.topMargin: 8

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4

            Text {
                text: "Purge Neural Data"
                font.family: Fonts.headlineFamily
                font.pixelSize: Fonts.title
                font.bold: true
                color: "#ff716c" // Theme.error
            }

            Text {
                text: "Permanently delete all locally cached transcriptions and audio buffers."
                font.family: Fonts.bodyFamily
                font.pixelSize: Fonts.normal
                color: Theme.textOnSurfaceVariant
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
        }

        Rectangle {
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.preferredWidth: 160
            Layout.preferredHeight: 48
            radius: 12
            color: "transparent"
            border.color: Qt.rgba(255, 113, 108, 0.3) // Theme.error with alpha

            Text {
                anchors.centerIn: parent
                text: "Purge All Data"
                font.family: Fonts.headlineFamily
                font.pixelSize: Fonts.normal
                font.bold: true
                color: "#ff716c" // Theme.error
            }
            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                hoverEnabled: true
                onEntered: parent.color = Qt.rgba(255, 113, 108, 0.1)
                onExited: parent.color = "transparent"
                onPressed: parent.scale = 0.95
                onReleased: parent.scale = 1.0
                onClicked: root.purgeClicked()
            }
            Behavior on scale { NumberAnimation { duration: 100 } }
        }
    }
}
