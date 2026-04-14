import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify

Rectangle {
    id: root
    property string icon
    property color iconColor
    property string title
    property string description

    signal clicked()

    Layout.preferredHeight: 120
    color: Theme.surfaceContainerLow
    radius: 24

    RowLayout {
        anchors.fill: parent
        anchors.margins: 32
        spacing: 24

        Rectangle {
            width: 64
            height: 64
            radius: 16
            color: Theme.surfaceContainerHighest

            Text {
                anchors.centerIn: parent
                text: root.icon
                font.family: Fonts.iconFamily
                color: root.iconColor
                font.pixelSize: 32
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4

            Text {
                text: root.title
                font.family: Fonts.headlineFamily
                font.pixelSize: Fonts.title
                font.bold: true
                color: Theme.textOnSurface
            }
            Text {
                text: root.description
                font.family: Fonts.bodyFamily
                font.pixelSize: Fonts.normal
                color: Theme.textOnSurfaceVariant
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onEntered: parent.color = Theme.surfaceContainerHigh
        onExited: parent.color = Theme.surfaceContainerLow
        onClicked: root.clicked()
    }
}
