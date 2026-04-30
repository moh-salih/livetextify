import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import LiveTextify
import "../singletons"

Rectangle {
    id: root

    // Automatically switch to compact mode on small screens or in Clear Mode
    property bool isCompact: Theme.isClearMode || (Window.window ? Window.window.width < 900 : false)

    // Abstracted Routing State
    property string currentRoute: "Dashboard"
    signal navigateRequested(string route)

    // Use implicitWidth instead of width so parent Layouts reclaim the space
    implicitWidth: isCompact ? 80 : 256
    color: Theme.surfaceContainerLow

    Behavior on implicitWidth {
        NumberAnimation { duration: 250; easing.type: Easing.OutQuart }
    }

    Rectangle {
        width: 1
        height: parent.height
        anchors.right: parent.right
        color: Theme.outlineVariant
        opacity: 0.3
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 100

            // Expanded Header
            Column {
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                spacing: 4

                visible: opacity > 0
                opacity: root.isCompact ? 0 : 1
                Behavior on opacity { NumberAnimation { duration: 150 } }

                Text {
                    text: "LiveTextify"
                    font.family: Fonts.headlineFamily
                    font.pixelSize: 24
                    font.bold: true
                    color: Theme.primary
                }
                Text {
                    text: "NEURAL INTELLIGENCE"
                    font.family: Fonts.bodyFamily
                    font.pixelSize: 10
                    font.letterSpacing: 2
                    color: Theme.primary
                    opacity: 0.6
                }
            }

            // Compact Header (Icon Only)
            Text {
                anchors.centerIn: parent
                visible: opacity > 0
                opacity: root.isCompact ? 1 : 0
                Behavior on opacity { NumberAnimation { duration: 150 } }

                text: "vital_signs"
                font.family: Fonts.iconFamily
                font.pixelSize: 32
                color: Theme.primary
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.leftMargin: root.isCompact ? 12 : 16
            Layout.rightMargin: root.isCompact ? 12 : 16
            spacing: 8

            NavButton {
                icon: "dashboard"
                text: "Dashboard"
                isActive: root.currentRoute === "Dashboard"
                onClicked: root.navigateRequested("Dashboard")
            }
            NavButton {
                icon: "mic_none"
                text: "Sessions"
                isActive: root.currentRoute === "Sessions"
                onClicked: root.navigateRequested("Sessions")
            }
            NavButton {
                icon: "layers"
                text: "Model Library"
                isActive: root.currentRoute === "Model Library"
                onClicked: root.navigateRequested("Model Library")
            }
            NavButton {
                icon: "settings"
                text: "Settings"
                isActive: root.currentRoute === "Settings"
                onClicked: root.navigateRequested("Settings")
            }
        }

        // Pushes everything above to the top
        Item { Layout.fillHeight: true }
    }

    component NavButton: Rectangle {
        id: btn
        property string icon
        property string text
        property bool isActive: false
        signal clicked()

        Layout.fillWidth: true
        Layout.preferredHeight: 48

        color: isActive ? Theme.surfaceContainerHighest : (hoverArea.containsMouse ? Qt.rgba(Theme.surfaceContainerHighest.r, Theme.surfaceContainerHighest.g, Theme.surfaceContainerHighest.b, 0.5) : "transparent")
        radius: 12

        ToolTip.text: btn.text
        ToolTip.visible: hoverArea.containsMouse && root.isCompact
        ToolTip.delay: 400

        Rectangle {
            width: 4
            height: parent.height
            radius: 2
            anchors.left: parent.left
            color: Theme.primary
            visible: isActive
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: root.isCompact ? 0 : 16
            spacing: 12

            Text {
                text: btn.icon
                font.family: Fonts.iconFamily
                font.pixelSize: 20
                color: isActive ? Theme.primary : Theme.textOnSurface
                opacity: isActive ? 1.0 : 0.6

                Layout.alignment: root.isCompact ? Qt.AlignHCenter : Qt.AlignLeft
                Layout.fillWidth: root.isCompact
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                visible: !root.isCompact
                text: btn.text
                font.family: Fonts.bodyFamily
                font.pixelSize: 14
                font.bold: isActive
                color: isActive ? Theme.primary : Theme.textOnSurface
                opacity: isActive ? 1.0 : 0.6
                Layout.fillWidth: true
            }
        }

        MouseArea {
            id: hoverArea
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            hoverEnabled: true
            onClicked: btn.clicked()
        }

        Behavior on color { ColorAnimation { duration: 150 } }
    }
}
