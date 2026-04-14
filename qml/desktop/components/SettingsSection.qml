import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify

ColumnLayout {
    id: root
    property string title
    property color themeColor
    default property alias content: container.data

    spacing: 24

    RowLayout {
        spacing: 12
        Rectangle {
            width: 32
            height: 2
            color: root.themeColor
        }
        Text {
            text: root.title
            font.family: Fonts.headlineFamily
            font.pixelSize: Fonts.largeTitle
            font.bold: true
            color: Theme.textOnSurface
        }
    }

    Rectangle {
        Layout.fillWidth: true
        // FIX: Explicitly derive height without circular anchor bindings
        Layout.preferredHeight: container.implicitHeight + 48
        color: Theme.surfaceContainerLow
        radius: 16

        ColumnLayout {
            id: container
            x: 24
            y: 24
            // FIX: Explicitly calculate width instead of anchors.fill
            width: parent.width - 48
            spacing: 32
        }
    }
}
