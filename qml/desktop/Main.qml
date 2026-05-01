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

    visible: true
    title: "LiveTextify"
    color: Theme.background

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
        x = (Screen.width - width) / 2
        y = (Screen.height - height) / 2

        Navigator.stack = pageStack
    }

    // Mediator: Handle global error state from AppState
    Connections {
        target: AppState
        function onHasErrorChanged() {
            if (AppState.hasError) {
                globalErrorDialog.open()
            } else {
                globalErrorDialog.close()
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Components.Sidebar {
            id: appSidebar
            Layout.fillHeight: true
            Layout.preferredWidth: implicitWidth
            z: 2

            // Connect to Global Navigator here, instead of inside the Component
            currentRoute: Navigator.currentTitle
            onNavigateRequested: (route) => {
                if (route === "Dashboard") Navigator.goToDashboard()
                else if (route === "Sessions") Navigator.goToSessions()
                else if (route === "Model Library") Navigator.goToModelLibrary()
                else if (route === "Settings") Navigator.goToSettings()
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            StackView {
                id: pageStack
                anchors.fill: parent
                initialItem: Pages.DashboardPage{}

                replaceEnter: Transition {
                    NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 150; easing.type: Easing.OutQuad }
                }
                replaceExit: Transition {
                    NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 150; easing.type: Easing.InQuad }
                }
            }

            Components.TopBar {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                z: 10
            }
        }
    }

    Components.LoadingIndicator{
        id: globalLoader
    }

    // Global Error Reporting UI
    Components.MessageDialog {
        id: globalErrorDialog
        // Data flows downwards (Rules 4 & 5)
        dialogType: Components.MessageDialog.Error
        title: "System Error"
        text: AppState.lastErrorString
        primaryText: "Dismiss"

        // Actions flow upwards (Rules 3, 5, 6, & 7)
        onAccepted: {
            AppState.clearError()
        }

        onClosed: {
            // Failsafe: in case it was closed via ESC key or clicking outside
            if (AppState.hasError) {
                AppState.clearError()
            }
        }
    }

    Connections{
        target: SessionManager
        function onLastErrorChanged(){
            if(AppState.hasError){
                globalErrorDialog.open()
            }
        }
    }
}
