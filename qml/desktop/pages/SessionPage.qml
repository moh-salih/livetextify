import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify
import "../singletons"
import "../components" as Components

Page {
    id: root

    // --- Deferred Loading State ---
    property var deferredSessionModel: null

    Timer {
        id: loadTimer
        interval: 200 // Wait 200ms for the StackView transition to finish
        running: true
        onTriggered: root.deferredSessionModel = AppState.sessionSvc.sessions
    }

    function handleOpenSessionAction(sessionId) {
        AppState.openSession(sessionId)
        Navigator.goToLiveSession("Session Review")
    }

    background: Components.PageBackground {
        topColor: Theme.surfaceContainerHighest
        bottomColor: Theme.background
    }

    ScrollView {
        anchors.fill: parent
        clip: true
        contentWidth: availableWidth

        ColumnLayout {
            width: Math.min(1200, parent.width - 48)
            anchors.horizontalCenter: parent.horizontalCenter
            y: 32
            spacing: 40

            Components.PageHeader {
                Layout.fillWidth: true
                title: "Live Sessions"
                titleColor: Theme.primary
                titleSize: Fonts.largeTitle
            }

            Components.ActiveSessionCard {
                Layout.fillWidth: true
                visible: AppState.activeSession !== null
                sessionTitle: AppState.activeSession ? AppState.activeSession.title : ""

                onViewStreamClicked: {
                    if (AppState.activeSession) {
                        Navigator.goToLiveSession("Active Session")
                    }
                }
                onStopSessionClicked: AppState.closeActiveSession()
            }

            Components.SessionFilterBar {
                Layout.fillWidth: true
                visible: false
            }

            Components.HistoricalSessionsGrid {
                Layout.fillWidth: true
                visible: root.deferredSessionModel !== null
                sessionModel: root.deferredSessionModel

                onOpenSession: (sessionId) => root.handleOpenSessionAction(sessionId)
            }

            Item { Layout.preferredHeight: 64 }
        }
    }

    Components.LoadingIndicator {
        visible: root.deferredSessionModel === null
        titleText: "Loading history..."
    }
}
