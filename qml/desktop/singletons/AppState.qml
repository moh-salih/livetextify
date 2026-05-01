pragma Singleton
import QtQuick
import LiveTextify

QtObject {
    id: root

    // ── Core Managers (injected from C++) ─────────────────────────────────────
    readonly property var sessionMgr  : SessionManager
    readonly property var modelMgr    : ModelManager
    readonly property var settingsMgr : SettingsManager

    // ── Settings Sub-Managers ─────────────────────────────────────────────────
    readonly property var sttConfig   : SettingsManager.sttConfigManager
    readonly property var llmConfig   : SettingsManager.llmConfigManager
    readonly property var ragConfig   : SettingsManager.ragConfigManager
    readonly property var audioConfig : SettingsManager.audioConfigManager
    readonly property var embConfig   : SettingsManager.embConfigManager

    // ── Model Sub-Managers ────────────────────────────────────────────────────
    readonly property var sttModel    : ModelManager.stt
    readonly property var llmModel    : ModelManager.llm
    readonly property var embModel    : ModelManager.emb

    // ── Session Service ───────────────────────────────────────────────────────
    readonly property var sessionSvc          : SessionManager.sessionService
    readonly property var activeSession       : sessionSvc.activeSession
    readonly property var activeSessionConfig : sessionSvc.sessionConfig

    // ── Session / Recording State ─────────────────────────────────────────────
    readonly property int  sessionStatus     : SessionManager.sessionStatus
    readonly property bool isRecording       : SessionManager.isRecording
    readonly property bool isLlamaGenerating : SessionManager.isLlamaGenerating

    // ── Engine Statuses ───────────────────────────────────────────────────────
    readonly property int whisperStatus  : SessionManager.whisperStatus
    readonly property int llamaStatus    : SessionManager.llamaStatus
    readonly property int embedderStatus : SessionManager.embedderStatus

    // ── Derived engine readiness (convenience for UI enable/disable) ──────────
    // QtWhisper::Status::Ready == 2, QtLlama::Status::Ready == 2
    readonly property bool isSttReady      : whisperStatus  === 2
    readonly property bool isLlmReady      : llamaStatus    === 2
    readonly property bool isEmbedderReady : embedderStatus === 2
    readonly property bool isSessionReady  : activeSession !== null
                                          && isSttReady
                                          && isLlmReady

    // ── Error State ───────────────────────────────────────────────────────────
    readonly property bool   hasError       : SessionManager.hasError
    readonly property int    lastError      : SessionManager.lastError
    readonly property string lastErrorString: SessionManager.lastErrorString

    // ── Session Lifecycle ─────────────────────────────────────────────────────
    function startNewSession()      { sessionSvc.createSession() }
    function openSession(sessionId) { sessionSvc.openSessionById(sessionId) }
    function closeActiveSession()   {
        SessionManager.stopRecording()  // ensure mic is off before closing
        sessionSvc.closeSession()
    }

    // ── Recording ─────────────────────────────────────────────────────────────
    function toggleRecording()      { SessionManager.toggleRecording() }
    function startRecording()       { SessionManager.startRecording() }
    function stopRecording()        { SessionManager.stopRecording() }

    // ── Chat ──────────────────────────────────────────────────────────────────
    function sendChatMessage(text)  { sessionSvc.sendMessage(text) }
    function stopGeneration()       { SessionManager.stopGeneration() }

    // ── Model Management ──────────────────────────────────────────────────────
    function downloadModel(subMgr, index) { subMgr.downloadModel(index) }
    function cancelDownload(subMgr, url)  { subMgr.cancelDownload(url) }
    function deleteModel(subMgr, url)     { subMgr.deleteModel(url) }
    function selectModel(subMgr, index)   { subMgr.selectDefault(index) }
    function isSelectedModel(subMgr, url) { return subMgr.storagePath(url) === subMgr.selectedPath }

    // ── Error Management ──────────────────────────────────────────────────────
    function clearError()                 { SessionManager.clearError() }


}
