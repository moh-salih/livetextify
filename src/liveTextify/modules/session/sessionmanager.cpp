#include "sessionmanager.h"
#include "liveTextify/modules/services/sessionservice.h"
#include "liveTextify/modules/services/transcriptionservice.h"
#include "liveTextify/modules/services/audioservice.h"
#include "liveTextify/modules/services/ragservice.h"
#include "liveTextify/modules/services/chatservice.h"
#include "liveTextify/modules/services/databaseservice.h"
#include "liveTextify/modules/session/sessionsettings.h"
#include "liveTextify/core/Logger.h"
#include "liveTextify/core/AppError.h"

SessionManager::SessionManager(QObject* parent)
    : QObject(parent)
{
    mDatabaseService      = new DatabaseService(this);
    mSessionService       = new SessionService(mDatabaseService, this);
    mAudioService         = new AudioService(this);
    mTranscriptionService = new TranscriptionService(this);
    mRagService           = new RagService(mDatabaseService, this);
    mChatService          = new ChatService(mDatabaseService, this);

    SessionSettings* settings = mSessionService->settings();
    if(settings->systemPrompt().trimmed().isEmpty()){
        settings->setSystemPrompt(
            "You are liveTextify from Maveria.co. "
            "CRITICAL RULES FOR YOUR RESPONSES:\n"
            "- Do not use any emojis or emoticons\n"
            "- Do not use markdown, asterisks, underscores, backticks, or HTML tags\n"
            "- Do not use any formatting for emphasis\n"
            "- Use plain text only with standard punctuation\n"
            "- Be professional, helpful, and factual\n"
            "- Keep responses concise and well-structured\n"
            "- Use dashes or numbers for lists without extra formatting\n\n"
            "Remember: clean plain text only - no symbols beyond basic punctuation."
        );
    }



    // ── Settings → Services ───────────────────────────────────────────────────
    connect(settings, &SessionSettings::configChanged, mAudioService, &AudioService::onConfigChanged);
    connect(settings, &SessionSettings::configChanged, mTranscriptionService, &TranscriptionService::onConfigChanged);
    connect(settings, &SessionSettings::configChanged, mRagService, &RagService::onConfigChanged);
    connect(settings, &SessionSettings::configChanged, mChatService, &ChatService::onConfigChanged);
    // ── Debug logging — remove when no longer needed ──────────────────────────
    connect(settings, &SessionSettings::configChanged, this, [](const SessionConfig& cfg) {
        Logger::info("\n\n\n\n=== Session Config Changed ===");

        Logger::info("[STT]");
        Logger::info("  modelPath:          " + cfg.stt.modelPath);
        Logger::info("  language:           " + cfg.stt.language);
        Logger::info("  autoDetectLanguage: " + QString(cfg.stt.autoDetectLanguage ? "true" : "false"));
        Logger::info("  translate:          " + QString(cfg.stt.translate          ? "true" : "false"));
        Logger::info("  threadCount:        " + QString::number(cfg.stt.threadCount));
        Logger::info("  maxTokens:          " + QString::number(cfg.stt.maxTokens));
        Logger::info("  temperature:        " + QString::number(cfg.stt.temperature, 'f', 2));

        Logger::info("[LLM]");
        Logger::info("  modelPath:          " + cfg.llm.modelPath);
        Logger::info("  systemPrompt:       " + cfg.systemPrompt.left(60) + (cfg.systemPrompt.length() > 60 ? "..." : ""));
        Logger::info("  contextLength:      " + QString::number(cfg.llm.contextLength) + (cfg.llm.contextLength == 0 ? " (from model)" : ""));
        Logger::info("  maxTokens:          " + QString::number(cfg.llm.maxTokens));
        Logger::info("  temperature:        " + QString::number(cfg.llm.temperature,  'f', 2));
        Logger::info("  topP:               " + QString::number(cfg.llm.topP,          'f', 2));
        Logger::info("  topK:               " + QString::number(cfg.llm.topK));
        Logger::info("  repeatPenalty:      " + QString::number(cfg.llm.repeatPenalty, 'f', 2));
        Logger::info("  threadCount:        " + QString::number(cfg.llm.threadCount));
        Logger::info("  batchSize:          " + QString::number(cfg.llm.batchSize));
        Logger::info("  batchThreads:       " + QString::number(cfg.llm.batchThreads));

        Logger::info("[EMB]");
        Logger::info("  modelPath:          " + cfg.emb.modelPath);
        Logger::info("  nCtx:               " + QString::number(cfg.emb.nCtx));
        Logger::info("  nThreads:           " + QString::number(cfg.emb.nThreads));

        Logger::info("[RAG]");
        Logger::info("  enabled:            " + QString(cfg.enableRag ? "true" : "false"));
        Logger::info("  minChunkLength:     " + QString::number(cfg.rag.chunking.minChunkLength));
        Logger::info("  maxChunkLength:     " + QString::number(cfg.rag.chunking.maxChunkLength));
        Logger::info("  search.topK:        " + QString::number(cfg.rag.search.topK));
        Logger::info("  search.minSim:      " + QString::number(cfg.rag.search.minSimilarity, 'f', 2));

        Logger::info("[AUDIO]");
        Logger::info("  windowDurationMs:   " + QString::number(cfg.audio.windowDurationMs));
        Logger::info("  stepDurationMs:     " + QString::number(cfg.audio.stepDurationMs));
    });

    // ── Session lifecycle → Services ──────────────────────────────────────────
    connect(mSessionService, &SessionService::activeSessionChanged, mAudioService, &AudioService::onActiveSessionChanged);

    connect(mSessionService, &SessionService::activeSessionChanged, mTranscriptionService, &TranscriptionService::onActiveSessionChanged);

    connect(mSessionService, &SessionService::activeSessionChanged, mRagService, &RagService::onActiveSessionChanged);

    connect(mSessionService, &SessionService::activeSessionChanged, mChatService, &ChatService::onActiveSessionChanged);


    connect(mSessionService, &SessionService::activeSessionChanged, this, [this](Session*) { setSessionStatus(SessionStatus::Idle); });

    // ── Audio → Transcription pipeline ───────────────────────────────────────
    connect(mAudioService, &AudioService::audioWindowReady, mTranscriptionService, &TranscriptionService::processAudioWindow);

    // ── Transcription → RAG indexing ─────────────────────────────────────────
    connect(mSessionService, &SessionService::transcriptionUpdated, mRagService, &RagService::onTranscriptionUpdated);

    // ── Chat ──────────────────────────────────────────────────────────────────
    connect(mSessionService, &SessionService::userMessageReady, mChatService, &ChatService::onUserMessageReady);

    connect(mChatService, &ChatService::contextLengthResolved, settings, &SessionSettings::onContextLengthResolved);

    // ── RAG ↔ Chat bridge — ChatService stays decoupled from RagService ───────
    connect(mChatService, &ChatService::contextRequested, mRagService, &RagService::requestContext);

    connect(mRagService, &RagService::contextReady, mChatService, &ChatService::onContextReady);

    // ── Transcription flow ────────────────────────────────────────────────────
    connect(mTranscriptionService, &TranscriptionService::transcriptionUpdated, mSessionService, &SessionService::onTranscriptionUpdated);

    // ── Stop generation ───────────────────────────────────────────────────────
    connect(mSessionService, &SessionService::stopGenerationRequested, mChatService, &ChatService::stopGeneration);

    // ── Status signals — pure forwards, no caching ────────────────────────────
    connect(mAudioService, &AudioService::isRecordingChanged, this, &SessionManager::isRecordingChanged);

    connect(mTranscriptionService, &TranscriptionService::isProcessingChanged, this, &SessionManager::isTranscribingChanged);

    connect(mTranscriptionService, &TranscriptionService::whisperStatusChanged, this, &SessionManager::whisperStatusChanged);

    connect(mChatService, &ChatService::isLlamaGeneratingChanged, this, &SessionManager::isLlamaGeneratingChanged);

    connect(mChatService, &ChatService::llamaStatusChanged, this, &SessionManager::llamaStatusChanged);

    connect(mRagService, &RagService::embedderStatusChanged, this, &SessionManager::embedderStatusChanged);

    // ── contextLength reset on model unload ───────────────────────────────────
    connect(mChatService, &ChatService::llamaStatusChanged,
            this, [this](QtLlama::Status s) {
                if (s == QtLlama::Status::Idle || s == QtLlama::Status::Error)
                    mSessionService->settings()->onContextLengthResolved(0);
            });

    // ── Error wiring ──────────────────────────────────────────────────────────
    connect(mAudioService, &AudioService::audioErrorOccurred,
            this, [this](QtAudioCapture::Error e) {
                Logger::error(QtAudioCapture::errorToString(e));
                reportError(LiveTextify::fromAudioError(e));
            });

    connect(mTranscriptionService, &TranscriptionService::errorOccurred,
            this, [this](QtWhisper::Error e) {
                Logger::error(QtWhisper::errorToString(e));
                reportError(LiveTextify::fromWhisperError(e));
            });

    connect(mChatService, &ChatService::llamaErrorOccurred,
            this, [this](QtLlama::Error e) {
                Logger::error(QtLlama::errorToString(e));
                reportError(LiveTextify::fromLlamaError(e, false));
            });

    connect(mRagService, &RagService::embedderErrorOccurred,
            this, [this](QtLlama::Error e) {
                Logger::error(QtLlama::errorToString(e));
                reportError(LiveTextify::fromLlamaError(e, true));
            });

    connect(mRagService, &RagService::errorOccurred,
            this, [this](QtRag::Error e) {
                Logger::error(QtRag::errorToString(e));
                reportError(LiveTextify::fromRagError(e));
            });

    // ── Reload signals ────────────────────────────────────────────────────────
    wireReloadSignals();

    // ── Load persisted sessions ───────────────────────────────────────────────
    mSessionService->loadHistory();
}

SessionManager::~SessionManager() = default;

// ── Direct service getters ────────────────────────────────────────────────────

bool SessionManager::isRecording()       const { return mAudioService->isRecording(); }
bool SessionManager::isTranscribing()    const { return mTranscriptionService->isProcessing(); }
bool SessionManager::isLlamaGenerating() const { return mChatService->isGenerating(); }
int  SessionManager::whisperStatus()     const { return static_cast<int>(mTranscriptionService->whisperStatus()); }
int  SessionManager::llamaStatus()       const { return static_cast<int>(mChatService->llamaStatus()); }
int  SessionManager::embedderStatus()    const { return static_cast<int>(mRagService->embedderStatus()); }

// ── Reload ────────────────────────────────────────────────────────────────────

void SessionManager::wireReloadSignals() {
    SessionSettings* settings = mSessionService->settings();

    connect(mTranscriptionService, &TranscriptionService::reloadRequired, settings, &SessionSettings::reloadRequired);

    connect(mChatService, &ChatService::reloadRequired, settings, &SessionSettings::reloadRequired);

    connect(mRagService, &RagService::reloadRequired, settings, &SessionSettings::reloadRequired);

    connect(settings, &SessionSettings::reloadApproved, mTranscriptionService, &TranscriptionService::reloadModels);

    connect(settings, &SessionSettings::reloadApproved, mChatService, &ChatService::reloadModels);

    connect(settings, &SessionSettings::reloadApproved, mRagService, &RagService::reloadModels);
}

// ── Recording ─────────────────────────────────────────────────────────────────

void SessionManager::startRecording()  { mAudioService->start(); }
void SessionManager::stopRecording()   { mAudioService->stop(); }

void SessionManager::toggleRecording() {
    isRecording() ? mAudioService->stop() : mAudioService->start();
}

void SessionManager::stopGeneration() { mChatService->stopGeneration(); }

// ── Session status ────────────────────────────────────────────────────────────

void SessionManager::setSessionStatus(SessionStatus s) {
    if (mSessionStatus == s) return;
    mSessionStatus = s;
    emit sessionStatusChanged();
}

// ── Error ─────────────────────────────────────────────────────────────────────

void SessionManager::reportError(LiveTextify::AppError error) {
    mLastError = error;
    mHasError  = true;
    emit lastErrorChanged();
}

void SessionManager::clearError() {
    mHasError = false;
    emit lastErrorChanged();
}
