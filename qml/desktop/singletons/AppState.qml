pragma Singleton
import QtQuick
import LiveTextify

QtObject {
    id: root

    // ── Hardware Resources ─────────────────────────────────────
    readonly property var resourceMonitor : ResourceMonitor

    // ── Core Managers (injected from C++) ─────────────────────────────────────
    readonly property var sessionMgr : SessionManager
    readonly property var modelMgr   : ModelManager

    // ── Session Service & Settings ────────────────────────────────────────────
    readonly property var sessionSvc     : SessionManager.sessionService
    readonly property var sessionSettings: sessionSvc.settings
    readonly property var activeSession  : sessionSvc.activeSession

    // ── Model Sub-Managers ────────────────────────────────────────────────────
    readonly property var sttModel : ModelManager.stt
    readonly property var llmModel : ModelManager.llm
    readonly property var embModel : ModelManager.emb

    // ── Session / Recording State ─────────────────────────────────────────────
    readonly property int  sessionStatus     : SessionManager.sessionStatus
    readonly property bool isRecording       : SessionManager.isRecording
    readonly property bool isLlamaGenerating : SessionManager.isLlamaGenerating

    // ── Engine Statuses ───────────────────────────────────────────────────────
    readonly property int whisperStatus  : SessionManager.whisperStatus
    readonly property int llamaStatus    : SessionManager.llamaStatus
    readonly property int embedderStatus : SessionManager.embedderStatus
    readonly property bool isTranscribing : SessionManager.isTranscribing

    // ── Engine State ───────────────────────────────────────────────────────
    readonly property int llamaResolvedContextLength: sessionSettings.resolvedContextLength

    // ── Derived engine readiness (convenience for UI enable/disable) ──────────
    readonly property bool isSttReady      : whisperStatus  === 2
    readonly property bool isLlmReady      : llamaStatus    === 2
    readonly property bool isEmbedderReady : embedderStatus === 2
    readonly property bool isSessionReady  : activeSession  !== null && isSttReady && isLlmReady

    // ── Error State ───────────────────────────────────────────────────────────
    readonly property bool   hasError        : SessionManager.hasError
    readonly property int    lastError       : SessionManager.lastError
    readonly property string lastErrorString : SessionManager.lastErrorString

    // ── Session Lifecycle ─────────────────────────────────────────────────────
    function startNewSession(title = "New Session") {
        let s = sessionSvc.createSession();
        if (s && title !== "New Session" && title !== "") {
            s.title = title;
        }
    }

    function openSession(sessionId) { sessionSvc.openSessionById(sessionId) }
    function closeActiveSession()   {
        SessionManager.stopRecording()
        sessionSvc.closeSession()
    }

    // ── Recording ─────────────────────────────────────────────────────────────
    function toggleRecording() { SessionManager.toggleRecording() }
    function startRecording()  { SessionManager.startRecording() }
    function stopRecording()   { SessionManager.stopRecording() }

    // ── Chat ──────────────────────────────────────────────────────────────────
    function sendChatMessage(text) { sessionSvc.sendMessage(text) }
    function stopGeneration()      { SessionManager.stopGeneration() }

    // ── Model Management ──────────────────────────────────────────────────────
    function downloadModel(subMgr, index) { subMgr.downloadModel(index) }
    function cancelDownload(subMgr, url)  { subMgr.cancelDownload(url) }
    function deleteModel(subMgr, url)     { subMgr.deleteModel(url) }
    function selectModel(subMgr, index)   { subMgr.selectDefault(index) }
    function isSelectedModel(subMgr, url) { return subMgr.storagePath(url) === subMgr.selectedPath }

    // ── Settings Management ───────────────────────────────────────────────────
    function resetSessionSettings() {
        sessionSettings.resetToDefaults()
    }

    // ── Error Management ──────────────────────────────────────────────────────
    function clearError() { SessionManager.clearError() }
    function confirmReload() { SessionManager.reloadModels() }
}
