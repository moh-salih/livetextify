pragma Singleton
import QtQuick
import LiveTextify

QtObject {
    id: root

    // --- Core Managers (Injected from C++) ---
    readonly property var sessionMgr: SessionManager
    readonly property var aiSvc: SessionManager.ai
    readonly property var audioSvc: SessionManager.audio
    readonly property var sessionSvc: SessionManager.sessions
    readonly property var questionsSvc: SessionManager.questions

    readonly property var whisper: WhisperManager
    readonly property var llama: LlamaManager
    readonly property var embedding: EmbeddingManager

    readonly property var settingsMgr   : SettingsManager
    readonly property var sttConfig     : SettingsManager.sttConfigManager
    readonly property var llmConfig     : SettingsManager.llmConfigManager
    readonly property var ragConfig     : SettingsManager.ragConfigManager
    readonly property var audioConfig   : SettingsManager.audioConfigManager
    readonly property var embConfig     : SettingsManager.embConfigManager

    // --- Global State ---
    readonly property var activeSession: sessionSvc.activeSession

    // Derived Engine States
    readonly property int sttStatus:    aiSvc.sttSession.status
    readonly property int llmStatus:    aiSvc.llmSession.status
    readonly property int embStatus:    aiSvc.embSession.status

    readonly property bool isRecording: audioSvc.isRecording

    // --- Session Lifecycle Actions ---
    function startNewSession() {
        // Enforce strict memory teardown before creating a new session
        if (activeSession) {
            closeActiveSession();
        }

        let configMap = {
            "title": "New Transcription Session",
            "sttModelPath": whisper.defaultModelPath,
            "llmModelPath": llama.defaultModelPath,
            "embeddingModelPath": embedding.defaultModelPath,
            "enableRag": true
        }
        sessionSvc.createSession(configMap);
    }

    function openSession(sessionId) {
        if (activeSession && activeSession.sessionID === sessionId) return;

        // Enforce strict memory teardown before switching to another session
        if (activeSession) {
            closeActiveSession();
        }

        sessionSvc.openSessionById(sessionId);
    }

    function closeActiveSession() {
        if (isRecording) {
            stopRecording();
        }
        // Closes session, signals nullptr to AiService, and drops models from RAM
        sessionSvc.closeSession();
    }

    // --- Hardware / Recording Actions ---
    function startRecording() {
        if (activeSession) {
            audioSvc.startRecording();
            aiSvc.sttSession.startInference();
        }
    }

    function stopRecording() {
        audioSvc.stopRecording();
        aiSvc.sttSession.stopInference();
    }

    // --- Chat Actions ---
    function sendChatMessage(text) {
        if (activeSession) {
            activeSession.sendMessage(text);
        }
    }

    // --- Insights Actions ---
    function refreshQuestions(count = 3) {
        questionsSvc.generateQuestions(count);
    }


}
