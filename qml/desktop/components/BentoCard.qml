import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify

Rectangle {
    id: root
    property string icon
    property color iconColor
    property string badgeText
    property string title
    property string value

    color: Theme.surfaceContainerLow
    radius: 24

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24

        RowLayout {
            Layout.fillWidth: true

            Rectangle {
                width: 48
                height: 48
                radius: 12
                color: Theme.surfaceContainerHighest

                Text {
                    anchors.centerIn: parent
                    text: root.icon
                    font.family: Fonts.iconFamily
                    color: root.iconColor
                    font.pixelSize: Fonts.largeTitle
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Text {
                text: root.badgeText
                font.family: Fonts.bodyFamily
                font.pixelSize: Fonts.tiny
                font.bold: true
                color: root.iconColor
                font.letterSpacing: 1
            }
        }

        Item {
            Layout.fillHeight: true
        }

        ColumnLayout {
            spacing: 4
            Text {
                text: root.title
                font.family: Fonts.bodyFamily
                font.pixelSize: Fonts.normal
                font.bold: true
                color: Theme.textOnSurfaceVariant
            }
            Text {
                text: root.value
                font.family: Fonts.headlineFamily
                font.pixelSize: Fonts.displaySmall
                font.bold: true
                color: Theme.textOnSurface
            }
        }
    }
}
