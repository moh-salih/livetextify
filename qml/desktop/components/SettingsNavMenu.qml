import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify

Rectangle {
    id: root

    property int currentIndex: 0
    property var tabs: []

    signal tabClicked(int index)

    Layout.fillWidth: true
    Layout.preferredHeight: layout.implicitHeight + 48
    color: Theme.surfaceContainerLow
    radius: 12
    border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.1)

    ColumnLayout {
        id: layout
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 24
        spacing: 4

        Text {
            text: "CONFIGURATION GROUPS"
            font.family: Fonts.bodyFamily
            font.pixelSize: Fonts.small
            font.bold: true
            font.letterSpacing: 2
            color: Theme.primary
            Layout.bottomMargin: 16
            visible: false // Not needed for now
        }


        Repeater{
            model: root.tabs
            delegate: NavItem{
                tabIndex: index
                visible: modelData.visible
                icon: modelData.icon
                text: modelData.text
            }
        }
    }

    component NavItem: Rectangle {
        id: navItem
        property int tabIndex
        property string icon
        property string text

        property bool isActive: root.currentIndex === tabIndex

        Layout.fillWidth: true
        Layout.preferredHeight: 48
        radius: 8

        color: isActive ? Theme.surfaceContainerHighest : (hoverArea.containsMouse ? Theme.surfaceContainerHigh : "transparent")

        RowLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 12

            Text {
                text: navItem.icon
                font.family: Fonts.iconFamily
                font.pixelSize: Fonts.title
                color: isActive ? Theme.primary : Theme.textOnSurfaceVariant
            }
            Text {
                text: navItem.text
                font.family: Fonts.bodyFamily
                font.pixelSize: Fonts.normal
                font.bold: isActive
                color: isActive ? Theme.primary : Theme.textOnSurfaceVariant
                Layout.fillWidth: true
            }
            Text {
                text: "chevron_right"
                font.family: Fonts.iconFamily
                font.pixelSize: Fonts.normal
                color: isActive ? Theme.primary : Theme.textOnSurfaceVariant
            }
        }

        MouseArea {
            id: hoverArea
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            hoverEnabled: true
            onClicked: root.tabClicked(navItem.tabIndex)
        }

        Behavior on color { ColorAnimation { duration: 150 } }
    }
}
