import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify

RowLayout {
    id: root
    spacing: 16

    // Filter Chips
    RowLayout {
        spacing: 12

        FilterChip { label: "All Sessions"; isActive: true }
        FilterChip { label: "Strategic" }
        FilterChip { label: "Neural Unit" }
        FilterChip { label: "Client Briefs" }
    }

    Item { Layout.fillWidth: true }

    // Icon Buttons
    RowLayout {
        spacing: 12

        IconButton { iconText: "filter_list" }
        IconButton { iconText: "sort" }
    }

    component FilterChip: Rectangle {
        property string label
        property bool isActive: false

        Layout.preferredHeight: 36
        Layout.preferredWidth: chipText.implicitWidth + 32
        radius: 18
        color: isActive ? Theme.primary : Qt.rgba(Theme.surfaceContainerHigh.r, Theme.surfaceContainerHigh.g, Theme.surfaceContainerHigh.b, 0.4)
        border.color: isActive ? "transparent" : Qt.rgba(255, 255, 255, 0.05)

        Text {
            id: chipText
            anchors.centerIn: parent
            text: label
            font.family: Fonts.bodyFamily
            font.pixelSize: Fonts.small
            font.bold: true
            color: isActive ? Theme.textOnPrimary : Theme.textOnSurfaceVariant
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            hoverEnabled: true
            onEntered: if(!isActive) parent.color = Qt.rgba(Theme.surfaceContainerHighest.r, Theme.surfaceContainerHighest.g, Theme.surfaceContainerHighest.b, 0.6)
            onExited: if(!isActive) parent.color = Qt.rgba(Theme.surfaceContainerHigh.r, Theme.surfaceContainerHigh.g, Theme.surfaceContainerHigh.b, 0.4)
        }
    }

    component IconButton: Rectangle {
        property string iconText

        Layout.preferredHeight: 40
        Layout.preferredWidth: 40
        radius: 12
        color: Qt.rgba(255, 255, 255, 0.05)
        border.color: Qt.rgba(255, 255, 255, 0.05)

        Text {
            anchors.centerIn: parent
            text: iconText
            font.family: Fonts.iconFamily
            font.pixelSize: Fonts.heading
            color: Theme.textOnSurfaceVariant
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            hoverEnabled: true
            onEntered: parent.color = Qt.rgba(255, 255, 255, 0.1)
            onExited: parent.color = Qt.rgba(255, 255, 255, 0.05)
        }
    }
}
