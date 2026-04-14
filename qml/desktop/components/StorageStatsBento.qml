import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify

Rectangle {
    id: root
    color: Theme.surfaceContainerHighest
    radius: 24
    clip: true

    // FIX: Provide explicit baseline dimensions so the Grid doesn't collapse
    implicitHeight: 280
    implicitWidth: 300

    // Background Glow
    Rectangle {
        width: 200
        height: 200
        radius: 100
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: -50
        anchors.rightMargin: -50
        color: Qt.rgba(Theme.secondary.r, Theme.secondary.g, Theme.secondary.b, 0.15)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 32

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Text {
                text: "bolt"
                font.family: Fonts.iconFamily
                font.pixelSize: Fonts.largeTitle
                color: Theme.secondary
            }

            Item { Layout.fillWidth: true }

            Text {
                text: "PEAK PERFORMANCE"
                font.family: Fonts.bodyFamily
                font.pixelSize: Fonts.tiny
                font.bold: true
                color: Theme.secondary
                font.letterSpacing: 1
            }
        }

        Item { Layout.fillHeight: true }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 12

            Text {
                text: "STORAGE UTILIZATION"
                font.family: Fonts.bodyFamily
                font.pixelSize: Fonts.tiny
                font.bold: true
                color: Qt.rgba(Theme.secondary.r, Theme.secondary.g, Theme.secondary.b, 0.6)
                font.letterSpacing: 1
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 8
                radius: 4
                color: Theme.surfaceContainerLowest

                Rectangle {
                    width: parent.width * 0.65
                    height: parent.height
                    radius: 4
                    color: Theme.secondary
                }
            }

            Text {
                text: "42.8 GB <font color='" + Qt.rgba(255, 255, 255, 0.5) + "' size='4'>/ 64 GB</font>"
                font.family: Fonts.headlineFamily
                font.pixelSize: Fonts.largeTitle
                font.bold: true
                color: Theme.textOnSurface
                textFormat: Text.RichText
            }
        }
    }
}
