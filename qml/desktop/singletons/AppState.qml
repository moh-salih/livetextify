pragma Singleton
import QtQuick
import LiveTextify

QtObject {
    id: root

    // --- Core Managers (Injected from C++) ---
    readonly property var sessionMgr    : SessionManager
    readonly property var modelMgr      : ModelManager
    readonly property var settingsMgr   : SettingsManager

    // --- Settings Sub-Managers ---
    readonly property var sttConfig     : SettingsManager.sttConfigManager
    readonly property var llmConfig     : SettingsManager.llmConfigManager
    readonly property var ragConfig     : SettingsManager.ragConfigManager
    readonly property var audioConfig   : SettingsManager.audioConfigManager
    readonly property var embConfig     : SettingsManager.embConfigManager

    // --- Model Sub-Managers ---
    readonly property var sttModel      : ModelManager.stt
    readonly property var llmModel      : ModelManager.llm
    readonly property var embModel      : ModelManager.emb

    // --- Session State (via SessionManager facade) ---
    readonly property var sessionSvc          : SessionManager.sessionService
    readonly property var activeSession       : sessionSvc.activeSession
    readonly property var activeSessionConfig : sessionSvc.sessionConfig
    readonly property int sessionStatus       : SessionManager.sessionStatus
    readonly property bool isRecording        : SessionManager.isRecording
    readonly property int whisperStatus       : SessionManager.whisperStatus
    readonly property int llamaStatus         : SessionManager.llamaStatus
    readonly property bool isLlamaGenerating  : SessionManager.isLlamaGenerating
    readonly property int embedderStatus      : SessionManager.embedderStatus

    // --- Session Lifecycle ---
    function startNewSession()          { sessionSvc.createSession() }
    function openSession(sessionId)     { sessionSvc.openSessionById(sessionId) }
    function closeActiveSession()       {
        SessionManager.stopRecording()  // ensure mic is off before closing
        sessionSvc.closeSession()
    }

    // --- Recording ---
    function toggleRecording()          { SessionManager.toggleRecording() }

    // --- Chat ---
    function sendChatMessage(text)      { sessionSvc.sendMessage(text) }
    function stopLlmGeneration()        { sessionSvc.stopGenerating() }

    // --- Model Management ---
    function downloadModel(subMgr, index)  { subMgr.downloadModel(index) }
    function cancelDownload(subMgr, url)   { subMgr.cancelDownload(url) }
    function deleteModel(subMgr, url)      { subMgr.deleteModel(url) }
    function selectModel(subMgr, index)    { subMgr.selectDefault(index) }
    function isSelectedModel(subMgr, url)  { return subMgr.storagePath(url) === subMgr.selectedPath }
}
