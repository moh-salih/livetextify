import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import LiveTextify

Rectangle {
    id: root
    property string pageTitle: "Dashboard"

    height: 64
    color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, 0.8)
    border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.1)
    border.width: 1

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
                // Window.window is the attached property available on any Item in Qt 6
                if (root.Window.window && root.Window.window.startSystemMove) {
                    root.Window.window.startSystemMove()
                } else {
                    console.warn("startSystemMove not available - using fallback")
                    // Fallback for older Qt versions
                    fallbackDrag.active = true
                }
            }
        }
    }

    // Fallback MouseArea for older Qt versions without startSystemMove
    MouseArea {
        id: fallbackDrag
        anchors.fill: parent
        enabled: false
        visible: false

        property point dragPosition: Qt.point(0, 0)

        onPressed: {
            dragPosition = Qt.point(mouse.x, mouse.y)
        }
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

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 24
        anchors.rightMargin: 8
        spacing: 24

        Item { Layout.fillWidth: true }

        // Dynamic Breadcrumb
        RowLayout {
            spacing: 24
            visible: !Theme.isClearMode

            Text {
                text: root.pageTitle
                font.family: Fonts.bodyFamily
                font.pixelSize: 12
                font.bold: true
                color: Theme.primary

                Layout.maximumWidth: 400
                elide: Text.ElideRight

                Rectangle {
                    width: parent.width
                    height: 2
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: -6
                    color: Theme.primary
                }
            }
        }

        Item { Layout.preferredWidth: 16 }

        // Window Controls (Minimize, Maximize, Close)
        RowLayout {
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
    }

    component IconButton: Rectangle {
        property string iconText
        width: 36
        height: 36
        radius: 12
        color: "transparent"

        Text {
            anchors.centerIn: parent
            text: parent.iconText
            font.family: Fonts.iconFamily
            font.pixelSize: 20
            color: Theme.textOnSurface
            opacity: 0.7
        }
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onEntered: parent.color = Theme.surfaceContainerHighest
            onExited: parent.color = "transparent"
        }
    }

    component WindowControlButton: Rectangle {
        property string iconText
        property bool isClose: false
        signal clicked()

        Layout.preferredWidth: 46
        Layout.preferredHeight: 32
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
            onEntered: parent.color = isClose ? "#d7383b" : Qt.rgba(255, 255, 255, 0.1)
            onExited: parent.color = "transparent"
            onClicked: parent.clicked()
        }
        Behavior on color { ColorAnimation { duration: 100 } }
    }
}
