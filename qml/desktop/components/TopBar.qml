import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import LiveTextify
import "../singletons"

Rectangle {
    id: root

    // --- Action Signals ---
    signal closeRequested()

    height: 48
    color: "transparent"

    // --- Native Frameless Window Dragging ---
    TapHandler {
        onTapped: if (tapCount === 2) toggleMaximized()
        gesturePolicy: TapHandler.DragThreshold
    }

    DragHandler {
        id: dragHandler
        target: null
        onActiveChanged: {
            if (active) {
                if (root.Window.window && root.Window.window.startSystemMove) {
                    root.Window.window.startSystemMove()
                } else {
                    fallbackDrag.active = true
                }
            }
        }
    }

    MouseArea {
        id: fallbackDrag
        anchors.fill: parent
        enabled: false
        visible: false

        property point dragPosition: Qt.point(0, 0)

        onPressed: dragPosition = Qt.point(mouse.x, mouse.y)
        onPositionChanged: {
            if (pressedButtons === Qt.LeftButton) {
                var win = root.Window.window
                if (win) {
                    win.x += mouse.x - dragPosition.x
                    win.y += mouse.y - dragPosition.y
                }
            }
        }
    }

    function toggleMaximized() {
        const win = root.Window.window
        if (!win) return
        win.visibility === Window.Maximized ? win.showNormal() : win.showMaximized()
    }
    // ----------------------------------------

    // Action and Window Controls docked to the far right
    RowLayout {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.rightMargin: 8
        spacing: 4

        // --- App Actions ---
        TopBarButton {
            iconText: "layers"
            tooltipText: "Model Library"
            onClicked: Navigator.goToModelLibrary()
        }

        TopBarButton {
            iconText: "settings"
            tooltipText: "Settings"
            onClicked: Navigator.goToSettings()
        }

        TopBarButton {
            iconText: "account_circle"
            tooltipText: "Profile"
            onClicked:  Navigator.goToProfile();
        }

        // Vertical Divider
        Rectangle {
            Layout.preferredWidth: 1
            Layout.preferredHeight: 20
            Layout.alignment: Qt.AlignVCenter
            Layout.leftMargin: 8
            Layout.rightMargin: 8
            color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.4)
        }

        // --- Window Controls ---
        TopBarButton {
            iconText: "remove"
            tooltipText: "Minimize"
            onClicked: {
                const win = root.Window.window
                if (win) win.showMinimized()
            }
        }

        TopBarButton {
            iconText: {
                const win = root.Window.window
                return (win?.visibility === Window.Maximized) ? "filter_none" : "crop_square"
            }
            tooltipText: (root.Window.window?.visibility === Window.Maximized) ? "Restore Down" : "Maximize"
            onClicked: root.toggleMaximized()
        }

        TopBarButton {
            iconText: "close"
            tooltipText: "Close"
            isClose: true
            onClicked: root.closeRequested() // Delegate to root page to trigger dialog
        }
    }

    component TopBarButton: Rectangle {
        property string iconText
        property string tooltipText: ""
        property bool isClose: false
        signal clicked()

        Layout.preferredWidth: 46
        Layout.preferredHeight: 32
        Layout.alignment: Qt.AlignVCenter
        radius: 6
        color: "transparent"

        ToolTip.text: tooltipText
        ToolTip.visible: hoverArea.containsMouse && parent.tooltipText !== ""
        ToolTip.delay: 400

        Text {
            anchors.centerIn: parent
            text: parent.iconText
            font.family: Fonts.iconFamily
            font.pixelSize: 18
            color: Theme.textOnSurface
            opacity: hoverArea.containsMouse ? 1.0 : 0.6
            Behavior on opacity { NumberAnimation { duration: 150 } }
        }

        MouseArea {
            id: hoverArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onEntered: parent.color = isClose ? "#d7383b" : Qt.rgba(Theme.surfaceContainerHighest.r, Theme.surfaceContainerHighest.g, Theme.surfaceContainerHighest.b, 0.8)
            onExited: parent.color = "transparent"
            onClicked: parent.clicked()
        }

        Behavior on color { ColorAnimation { duration: 100 } }
    }
}
