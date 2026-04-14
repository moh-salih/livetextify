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
            y: 32
            spacing: 40

            RowLayout {
                Layout.fillWidth: true
                Text {
                    text: "Live Sessions"
                    font.family: Fonts.headlineFamily
                    font.pixelSize: Fonts.largeTitle
                    font.bold: true
                    color: Theme.primary
                }
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

            Components.LoadingIndicator {
                Layout.fillWidth: true
                visible: root.deferredSessionModel === null
                text: "Loading history..."
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
}
