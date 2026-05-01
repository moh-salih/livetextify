#include "sessionmanager.h"
#include "liveTextify/modules/services/sessionservice.h"
#include "liveTextify/modules/services/transcriptionservice.h"
#include "liveTextify/modules/services/chatservice.h"
#include "liveTextify/modules/services/databaseservice.h"
#include "liveTextify/modules/settings/settingsmanager.h"
#include "liveTextify/core/Logger.h"
#include "liveTextify/core/AppError.h"

SessionManager::SessionManager(SettingsManager* settings, QObject* parent)
    : QObject(parent)
{
    mDatabaseService      = new DatabaseService(this);
    mSessionService       = new SessionService(mDatabaseService, this);
    mTranscriptionService = new TranscriptionService(this);
    mChatService          = new ChatService(mDatabaseService, this);

    // ── Settings → SessionService ─────────────────────────────────────────────
    connect(settings, &SettingsManager::configChanged,
            mSessionService, &SessionService::onGlobalConfigsUpdated);

    // ── Session lifecycle → Services ──────────────────────────────────────────
    connect(mSessionService, &SessionService::activeSessionChanged,
            mChatService, &ChatService::onActiveSessionChanged);

    connect(mSessionService, &SessionService::activeSessionChanged,
            mTranscriptionService, &TranscriptionService::onActiveSessionChanged);

    connect(mSessionService, &SessionService::activeSessionChanged,
            this, [this](Session*) { setSessionStatus(SessionStatus::Idle); });

    // ── Chat ──────────────────────────────────────────────────────────────────
    connect(mSessionService, &SessionService::userMessageReady, mChatService, &ChatService::onUserMessageReady);
    connect(mSessionService, &SessionService::activeSessionConfigChanged, mChatService, &ChatService::onActiveSessionConfigChanged);
    // ── Transcription flow ────────────────────────────────────────────────────
    connect(mTranscriptionService, &TranscriptionService::transcriptionUpdated, mSessionService, &SessionService::onTranscriptionUpdated);
    connect(mSessionService, &SessionService::activeSessionConfigChanged, mTranscriptionService, &TranscriptionService::onActiveSessionConfigChanged);

    connect(mSessionService, &SessionService::transcriptionUpdated,
            mChatService, &ChatService::onTranscriptionUpdated);

    // ── Recording state ───────────────────────────────────────────────────────
    connect(mTranscriptionService, &TranscriptionService::isRecordingChanged,
            this, [this]() {
                mIsRecording = mTranscriptionService->isRecording();
                if (mIsRecording)
                    setSessionStatus(SessionStatus::Recording);
                else if (mSessionStatus == SessionStatus::Recording)
                    setSessionStatus(SessionStatus::Paused);
                emit isRecordingChanged();
            });

    // ── Engine statuses ───────────────────────────────────────────────────────
    connect(mTranscriptionService, &TranscriptionService::whisperStatusChanged,
            this, [this](QtWhisper::Status s) {
                mWhisperStatus = s;
                emit whisperStatusChanged();
            });

    connect(mChatService, &ChatService::llamaStatusChanged,
            this, [this](QtLlama::Status s) {
                mLlamaStatus = s;
                emit llamaStatusChanged();
            });

    connect(mChatService, &ChatService::embedderStatusChanged,
            this, [this](QtLlama::Status s) {
                mEmbedderStatus = s;
                emit embedderStatusChanged();
            });

    // ── LLM generation state ──────────────────────────────────────────────────
    connect(mChatService, &ChatService::isLlamaGeneratingChanged,
            this, [this](bool generating) {
                if (mIsLlamaGenerating == generating) return;
                mIsLlamaGenerating = generating;
                emit isLlamaGeneratingChanged();
            });

    // ── Error wiring ──────────────────────────────────────────────────────────
    connect(mTranscriptionService, &TranscriptionService::errorOccurred,
            this, [this](QtWhisper::Error e) {
                Logger::error(QtWhisper::errorToString(e));
                reportError(LiveTextify::fromWhisperError(e));
            });

    connect(mTranscriptionService, &TranscriptionService::audioErrorOccurred,
            this, [this](QtAudioCapture::Error e) {
                Logger::error(QtAudioCapture::errorToString(e));
                reportError(LiveTextify::fromAudioError(e));
            });

    connect(mChatService, &ChatService::llamaErrorOccurred,
            this, [this](QtLlama::Error e) {
                Logger::error(QtLlama::errorToString(e));
                reportError(LiveTextify::fromLlamaError(e, false));
            });

    connect(mChatService, &ChatService::embedderErrorOccurred,
            this, [this](QtLlama::Error e) {
                Logger::error(QtLlama::errorToString(e));
                reportError(LiveTextify::fromLlamaError(e, true));
            });

    connect(mChatService, &ChatService::ragErrorOccurred,
            this, [this](QtRag::Error e) {
                Logger::error(QtRag::errorToString(e));
                reportError(LiveTextify::fromRagError(e));
            });

    // ── Load persisted sessions ───────────────────────────────────────────────
    mSessionService->loadHistory();
}

void SessionManager::setSessionStatus(SessionStatus s) {
    if (mSessionStatus == s) return;
    mSessionStatus = s;
    emit sessionStatusChanged();
}

void SessionManager::reportError(LiveTextify::AppError error) {
    mLastError = error;
    mHasError  = true;
    emit lastErrorChanged();
}

void SessionManager::clearError() {
    mHasError = false;
    emit lastErrorChanged();
}

void SessionManager::startRecording() {
    mTranscriptionService->startTranscription();
}

void SessionManager::stopRecording() {
    mTranscriptionService->stopTranscription();
}

void SessionManager::toggleRecording() {
    if (mIsRecording)
        mTranscriptionService->stopTranscription();
    else
        mTranscriptionService->startTranscription();
}

void SessionManager::stopGeneration() {
    mChatService->stopGeneration();
}

SessionManager::~SessionManager() = default;
