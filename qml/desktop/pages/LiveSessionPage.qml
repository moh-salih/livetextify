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

    // Force closing the session if the user navigates away using TopBar or Escape
    StackView.onRemoved: {
        AppState.closeActiveSession()
    }

    // Model name derivations using sessionConfig
    property string activeSttModelName: {
        if (AppState.sttModel && AppState.sttModel.selectedPath) {
            return AppState.sttModel.selectedPath.split('/').pop()
        }
        return "No STT Model"
    }

    property string activeLlmModelName: {
        if (AppState.llmModel && AppState.llmModel.selectedPath) {
            return AppState.llmModel.selectedPath.split('/').pop()
        }
        return "No LLM Model"
    }

    property string activeEmbModelName: {
        if (AppState.embModel && AppState.embModel.selectedPath) {
            return AppState.embModel.selectedPath.split('/').pop()
        }
        return "No Embedding Model"
    }

    Timer {
        id: sessionTimer
        interval: 1000
        running: AppState.isRecording
        repeat: true
        onTriggered: root.sessionSeconds++
    }

    function handleCloseSession() {
        Navigator.goBack() // Will trigger StackView.onRemoved to gracefully close the session
    }

    background: Components.PageBackground {
        topColor: Theme.surfaceContainerHigh
        bottomColor: Theme.background
    }

    // Add the Panel Overlay
    Components.SessionSettingsPanel {
        id: sessionSettingsPanel
        settings: SessionManager.sessionService.settings

        onResetDefaultsRequested: {
            AppState.resetSessionSettings()
        }
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
            isRecording: AppState.isRecording
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
                sessionSettingsPanel.open()
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
                    isRecording: AppState.isRecording
                }

                Components.LiveChatTab {
                    Layout.fillWidth: true; Layout.fillHeight: true
                    chatModel: AppState.activeSession ? AppState.activeSession.conversation.messages : null
                    isGenerating: AppState.isLlamaGenerating
                    onSendMessage: function(text) { AppState.sendChatMessage(text) }
                    onStopGeneration: function() { AppState.stopGeneration() }
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
                        Layout.maximumWidth: 160

                        onClicked: root.showChat = !root.showChat
                    }
                }
            }
        }
    }
}
