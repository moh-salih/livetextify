import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify
import QtWhisper
import QtLlama

import "components" as Components
import "pages" as Pages
import "singletons"

ApplicationWindow {
    id: root
    width: Screen.width * 0.8
    height: Screen.height * 0.9

    // Prevents the window from being resized into a broken layout state
    minimumWidth: 800
    minimumHeight: 600

    visible: true
    title: "LiveTextify"
    color: Theme.background

    flags: Qt.Window | Qt.CustomizeWindowHint | Qt.FramelessWindowHint

    // --- Standard App Shortcuts ---
    Shortcut {
        // 'Back' handles Android native back buttons as well
        sequences: ["Escape", "Back"]
        onActivated: {
            if (pageStack.depth > 1) {
                // If we are deep in the stack, pop backward
                Navigator.goBack()
            } else {
                // If we are on the root dashboard page, prompt to exit
                exitConfirmDialog.open()
            }
        }
    }

    Shortcut {
        sequence: "Ctrl+H"
        onActivated: Theme.isClearMode = !Theme.isClearMode
    }

    // --- Cross-Platform Split Screen Emulation ---
    Shortcut {
        sequences: ["Meta+Left", "Ctrl+Alt+Left"]
        onActivated: {
            if (root.visibility === Window.Maximized) root.showNormal()

            root.x = Screen.virtualX
            root.y = Screen.virtualY
            root.width = Screen.desktopAvailableWidth / 2
            root.height = Screen.desktopAvailableHeight
        }
    }

    Shortcut {
        sequences: ["Meta+Right", "Ctrl+Alt+Right"]
        onActivated: {
            if (root.visibility === Window.Maximized) root.showNormal()

            root.x = Screen.virtualX + (Screen.desktopAvailableWidth / 2)
            root.y = Screen.virtualY
            root.width = Screen.desktopAvailableWidth / 2
            root.height = Screen.desktopAvailableHeight
        }
    }

    Shortcut {
        sequences: ["Meta+Up", "Ctrl+Alt+Up"]
        onActivated: {
            root.showMaximized()
        }
    }

    Shortcut {
        sequences: ["Meta+Down", "Ctrl+Alt+Down"]
        onActivated: {
            if (root.visibility === Window.Maximized) {
                root.showNormal()
            } else {
                root.showMinimized()
            }
        }
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

        // Vertical Resource Monitor taking the place of the old Sidebar
        Components.ResourceMonitorBar {
            visible: !Theme.isClearMode
            Layout.fillHeight: true
            Layout.preferredWidth: 140
            z: 2
            ramUsed  : ResourceMonitor.ramUsedBytes  / (1024.0 * 1024 * 1024)
            ramTotal : ResourceMonitor.ramTotalBytes / (1024.0 * 1024 * 1024)
            cpuUsage : ResourceMonitor.cpuPercent
            threads  : ResourceMonitor.threadCount
            hasGpu   : ResourceMonitor.gpus.length > 0
            gpuName  : hasGpu ? ResourceMonitor.gpus[0].name       : ""
            gpuUsed  : hasGpu ? ResourceMonitor.gpus[0].usedBytes  / (1024.0 * 1024 * 1024) : 0.0
            gpuTotal : hasGpu ? ResourceMonitor.gpus[0].totalBytes / (1024.0 * 1024 * 1024) : 1.0
        }

        // Main Content Area
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            StackView {
                id: pageStack
                anchors.fill: parent
                initialItem: Pages.DashboardPage{}

                // Setup proper push/pop transitions for a smooth stack history feel
                pushEnter: Transition {
                    NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 150; easing.type: Easing.OutQuad }
                    NumberAnimation { property: "scale"; from: 0.95; to: 1.0; duration: 150; easing.type: Easing.OutQuad }
                }
                pushExit: Transition {
                    NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 150; easing.type: Easing.InQuad }
                }
                popEnter: Transition {
                    NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 150; easing.type: Easing.OutQuad }
                }
                popExit: Transition {
                    NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 150; easing.type: Easing.InQuad }
                    NumberAnimation { property: "scale"; from: 1.0; to: 0.95; duration: 150; easing.type: Easing.InQuad }
                }
            }

            Components.TopBar {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                z: 10
                // Wires up the TopBar close X button to the exit dialog
                onCloseRequested: exitConfirmDialog.open()
            }
        }
    }

    Components.LoadingIndicator{
        id: globalLoader
    }

    // Global Error Reporting UI
    Components.MessageDialog {
        id: globalErrorDialog
        dialogType: Components.MessageDialog.Error
        title: "System Error"
        text: AppState.lastErrorString
        primaryText: "Dismiss"

        onAccepted: AppState.clearError()
        onClosed: {
            if (AppState.hasError) AppState.clearError()
        }
    }

    // Exit Confirmation UI
    Components.MessageDialog {
        id: exitConfirmDialog
        dialogType: Components.MessageDialog.Warning
        title: "Exit Application"
        text: "Are you sure you want to exit LiveTextify? Active processes will be terminated."
        primaryText: "Exit"
        secondaryText: "Cancel"

        onAccepted: Qt.quit()
        // onRejected auto-closes, no extra code needed here
    }

    // --- Window Mediator Actions ---
    Components.WindowResizeGrips {
        anchors.fill: parent
        z: 100

        onResizeRequested: (edge) => {
            root.startSystemResize(edge)
        }
    }

    Connections{
        target: SessionManager
        function onLastErrorChanged(){
            if(AppState.hasError){
                globalErrorDialog.open()
            }
        }

        function onWhisperStatusChanged(){
            switch(SessionManager.whisperStatus){
                case QtWhisper.Status.Loading:  globalLoader.start("Initializing Speech Recognition", "Loading Whisper model for real-time voice transcription..."); break;
                default: globalLoader.stop();
            }
        }

        function onLlamaStatusChanged(){
            switch(SessionManager.llamaStatus){
                case QtLlama.Status.Loading:  globalLoader.start("Initializing Language Model", "Loading Llama model for intelligent chat and analysis..."); break;
                default: globalLoader.stop();
            }
        }

        function onEmbedderStatusChanged(){
            switch(SessionManager.embedderStatus){
                case QtLlama.Status.Loading:  globalLoader.start("Initializing Context Memory Model", "Loading embedding model to enable contextual recall..."); break;
                default: globalLoader.stop();
            }
        }
    }
}
