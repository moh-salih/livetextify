import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import LiveTextify
import "../singletons"
import "../components" as Components

Page {
    id: root

    // --- Page State ---
    property bool showChat: false
    property int sessionSeconds: 0

    // Model name derivations using sessionConfig
    property string activeSttModelName: AppState.sessionSvc.sessionConfig.sttModelPath
        ? AppState.sessionSvc.sessionConfig.sttModelPath.split('/').pop() : "No STT Model"

    property string activeLlmModelName: AppState.sessionSvc.sessionConfig.llmModelPath
        ? AppState.sessionSvc.sessionConfig.llmModelPath.split('/').pop() : "No LLM Model"

    // Timer logic brought to page level. Only ticks while status == 1 (Recording)
    Timer {
        id: sessionTimer
        interval: 1000
        running: AppState.sessionStatus === 1
        repeat: true
        onTriggered: root.sessionSeconds++
    }

    function handleCloseSession() {
        AppState.closeActiveSession()
        Navigator.goToDashboard()
    }

    background: Components.PageBackground {
        topColor: Theme.surfaceContainerHigh
        bottomColor: Theme.background
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // --- Mediator Pattern: Handle components entirely via bound properties and signals ---
        Components.UnifiedSessionHeader {
            Layout.fillWidth: true
            visible: !Theme.isClearMode

            // Push State Downward
            sessionTitle: AppState.activeSession ? AppState.activeSession.title : "No Active Session"
            sessionStatus: AppState.sessionStatus
            sessionSeconds: root.sessionSeconds

            // Context-sensitive active model label and list
            modelTypeLabel: root.showChat ? "LLM:" : "STT:"
            activeModelName: root.showChat ? root.activeLlmModelName : root.activeSttModelName
            availableModels: root.showChat ? AppState.llmModel.models : AppState.sttModel.models

            // Pull Actions Upward
            onTitleEdited: function(newTitle) {
                if (AppState.activeSession) {
                    AppState.activeSession.title = newTitle;
                }
            }
            onConfigClicked: {
                Navigator.goToSettings()
            }
            onModelSelected: function(index) {
                if (root.showChat) {
                    AppState.selectModel(AppState.llmModel, index)
                } else {
                    AppState.selectModel(AppState.sttModel, index)
                }
            }
            onToggleRecordingClicked: AppState.toggleRecording()
            onEndSessionClicked: root.handleCloseSession()
        }

        // Main Content Area & Right Sidebar
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            // 1. Primary Tab Content
            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentIndex: root.showChat ? 1 : 0

                Components.LiveTranscriptTab {
                    Layout.fillWidth: true; Layout.fillHeight: true
                    session: AppState.activeSession
                    isRecording: AppState.sessionStatus === 1
                }

                Components.LiveChatTab {
                    Layout.fillWidth: true; Layout.fillHeight: true
                    chatModel: AppState.activeSession ? AppState.activeSession.conversation.messages : null
                    isGenerating: AppState.isLlamaGenerating
                    onSendMessage: function(text) { AppState.sendChatMessage(text) }
                    onStopGeneration: function() { AppState.stopLlmGeneration() }
                }
            }

            // 2. Right Actions Sidebar
            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: 200
                color: Qt.rgba(Theme.surfaceContainerLow.r, Theme.surfaceContainerLow.g, Theme.surfaceContainerLow.b, 0.4)
                border.width: 1
                border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.1)

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 24
                    spacing: 24

                    // Push the button to the bottom
                    Item { Layout.fillHeight: true }

                    // View Toggle
                    Components.FloatingActionButton {
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                        icon: root.showChat ? "description" : "forum"
                        text: root.showChat ? "Transcript" : "Chat"

                        // Prevent the button from forcing the sidebar wider than it needs to be
                        Layout.maximumWidth: 160

                        onClicked: root.showChat = !root.showChat
                    }
                }
            }
        }
    }
}
