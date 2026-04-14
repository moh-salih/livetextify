import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify
import "../singletons"
import "../components" as Components

Page {
    id: root

    property int currentTabIndex: 0

    function handleTabClicked(index) {
        root.currentTabIndex = index
    }

    function handleCloseSession() {
        AppState.closeActiveSession()
        Navigator.goToDashboard()
    }

    // Neural gradient background matching the mockup
    background: Rectangle {
        gradient: Gradient {
            GradientStop { position: 0.0; color: Theme.surfaceContainerHigh }
            GradientStop { position: 1.0; color: Theme.background }
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Main Workspace
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            Components.SessionInfoHeader {
                Layout.fillWidth: true
                visible: !Theme.isClearMode
                sessionTitle: AppState.activeSession ? AppState.activeSession.title : "No Active Session"
                startTimeText: AppState.activeSession ? ("Started " + Qt.formatDateTime(AppState.activeSession.createdAt, "hh:mm • dd MMM yyyy")) : "Pending..."

                onTitleEdited: function(newTitle) {
                    if (AppState.activeSession) {
                        AppState.activeSession.title = newTitle;
                        console.log("Update session title to: ", AppState.activeSession.title, " from :", newTitle);
                    }
                }
            }

            // Horizontal Tab Bar
            Components.SessionBottomNav {
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 24
                Layout.bottomMargin: 8
                Layout.preferredHeight: 64
                Layout.preferredWidth: 420

                orientation: Qt.Horizontal
                currentTabIndex: root.currentTabIndex
                onTabClicked: function(index) { root.handleTabClicked(index) }
            }

            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentIndex: root.currentTabIndex

                Components.LiveTranscriptTab {
                    Layout.fillWidth: true; Layout.fillHeight: true
                    session: AppState.activeSession
                    isRecording: AppState.isRecording
                }

                Components.LiveChatTab {
                    Layout.fillWidth: true; Layout.fillHeight: true
                    chatModel: AppState.activeSession ? AppState.activeSession.conversation.messages : null
                    onSendMessage: function(text) { AppState.sendChatMessage(text) }
                }

                Components.LiveQATab {
                    visible: false
                    Layout.fillWidth: true; Layout.fillHeight: true
                    questionModel: AppState.questionsSvc.questionsList
                    onRefreshClicked: function() { AppState.refreshQuestions(3) }
                }
            }
        }

        // Right Controls Sidebar
        Components.SessionControlSidebar {
            Layout.fillHeight: true
            Layout.preferredWidth: 320
            visible: !Theme.isClearMode // Auto-hides right sidebar completely when entering Clear Mode

            isRecording: AppState.isRecording
            engineName: "Neural-V4"
            contextUsage: "12 / 128k"
            contextProgress: 0.09

            sttStatus: AppState.sttStatus
            llmStatus: AppState.llmStatus
            ragStatus: AppState.embStatus

            onToggleRecordingClicked: {
                if (AppState.isRecording) {
                    AppState.stopRecording()
                } else {
                    AppState.startRecording()
                }
            }
            onEndSessionClicked: root.handleCloseSession()
        }
    }
}
