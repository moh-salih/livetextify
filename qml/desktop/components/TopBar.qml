import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import LiveTextify
import "../singletons"

// Made completely transparent. It acts as an invisible hit-box for dragging
// and a container for the window controls.
Rectangle {
    id: root

    height: 48 // Slimmer height since it only holds window controls
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

    // Window Controls docked to the far right
    RowLayout {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.rightMargin: 8
        spacing: 0

        WindowControlButton {
            iconText: "remove"
            onClicked: {
                const win = root.Window.window
                if (win) win.showMinimized()
            }
        }
        WindowControlButton {
            iconText: {
                const win = root.Window.window
                return (win?.visibility === Window.Maximized) ? "filter_none" : "crop_square"
            }
            onClicked: root.toggleMaximized()
        }
        WindowControlButton {
            iconText: "close"
            isClose: true
            onClicked: Qt.quit()
        }
    }

    component WindowControlButton: Rectangle {
        property string iconText
        property bool isClose: false
        signal clicked()

        Layout.preferredWidth: 46
        Layout.preferredHeight: 32
        Layout.alignment: Qt.AlignVCenter
        radius: 6
        color: "transparent"

        Text {
            anchors.centerIn: parent
            text: parent.iconText
            font.family: Fonts.iconFamily
            font.pixelSize: 16
            color: Theme.textOnSurface
            opacity: hoverArea.containsMouse ? 1.0 : 0.6
        }
        MouseArea {
            id: hoverArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            // We use a stronger hover color here since the background might be complex
            onEntered: parent.color = isClose ? "#d7383b" : Qt.rgba(Theme.surfaceContainerHighest.r, Theme.surfaceContainerHighest.g, Theme.surfaceContainerHighest.b, 0.8)
            onExited: parent.color = "transparent"
            onClicked: parent.clicked()
        }
        Behavior on color { ColorAnimation { duration: 100 } }
    }
}
