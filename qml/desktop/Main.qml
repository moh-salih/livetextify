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
            if (AppState.hasError) {
                AppState.clearError()
            }
        }
    }

    // --- Window Mediator Actions ---
    // Handling the resize signal purely at the root page level
    Components.WindowResizeGrips {
        anchors.fill: parent
        z: 100 // Must be placed on top of all child content to capture edge hovers

        onResizeRequested: (edge) => {
            root.startSystemResize(edge) // Action initiated by mediator
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
