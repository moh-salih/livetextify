import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify
import "../components" as Components
import "../singletons"

Page {
    id: root

    // --- Deferred Loading State ---
    property var deferredSessionModel: null

    Timer {
        id: loadTimer
        interval: 200
        running: true
        onTriggered: root.deferredSessionModel = AppState.sessionSvc.sessions
    }

    function handleNewTranscription() {
        AppState.startNewSession()
        Navigator.goToLiveSession("Active Session")
    }

    background: Rectangle {
        gradient: Gradient {
            GradientStop { position: 0.0; color: Theme.surfaceContainerHighest }
            GradientStop { position: 1.0; color: Theme.background }
        }
    }

    ScrollView {
        anchors.fill: parent
        clip: true
        contentWidth: availableWidth

        ColumnLayout {
            width: Math.min(1200, parent.width - 48)
            anchors.horizontalCenter: parent.horizontalCenter
            y: 40
            spacing: 40

            Components.DashboardHero {
                Layout.fillWidth: true
                onNewTranscriptionClicked: root.handleNewTranscription()
            }

            // Components.LoadingIndicator {
            //     Layout.fillWidth: true
            //     visible: root.deferredSessionModel === null
            //     text: "Loading recent sessions..."
            // }

            // Components.DashboardWidgetGrid {
            //     Layout.fillWidth: true
            //     visible: root.deferredSessionModel !== null
            //     sessionModel: root.deferredSessionModel
            // }

            Item { Layout.preferredHeight: 40 }
        }
    }
}
