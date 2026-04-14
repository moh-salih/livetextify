import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify
import "components" as Components
import "pages" as Pages
import "singletons"

ApplicationWindow {
    id: root
    width: Screen.width * 0.8
    height: Screen.height * 0.9
    // Removed dynamic x/y bindings from here to prevent fighting the OS during maximize

    visible: true
    title: "LiveTextify"
    color: Theme.background

    // Added Qt.CustomizeWindowHint to prevent the OS from restoring the native title bar
    flags: Qt.Window | Qt.CustomizeWindowHint | Qt.FramelessWindowHint

    Shortcut {
        sequence: "Escape"
        onActivated: Qt.quit()
    }

    Shortcut {
        sequence: "Ctrl+H"
        onActivated: Theme.isClearMode = !Theme.isClearMode
    }

    Component.onCompleted: {
        // Set initial position here to center the window once on startup
        x = (Screen.width - width) / 2
        y = (Screen.height - height) / 2

        // Wire the Navigator singleton to the real StackView exactly once
        Navigator.stack = pageStack
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Components.Sidebar {
            id: appSidebar
            Layout.fillHeight: true
            // Explicitly bind the layout width to the animated implicitWidth
            Layout.preferredWidth: implicitWidth
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            Components.TopBar {
                Layout.fillWidth: true
                pageTitle: Navigator.currentTitle
            }

            StackView {
                id: pageStack
                Layout.fillWidth: true
                Layout.fillHeight: true
                initialItem: Pages.DashboardPage{}

                replaceEnter: Transition {
                    NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 150; easing.type: Easing.OutQuad }
                }
                replaceExit: Transition {
                    NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 150; easing.type: Easing.InQuad }
                }
            }
        }
    }
}
