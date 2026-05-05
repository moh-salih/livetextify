#include "liveTextify/modules/services/transcriptionservice.h"
#include "liveTextify/modules/session/session.h"
#include "liveTextify/core/Logger.h"

#include <QtWhisper/Session.h>
#include <QtWhisper/Engine.h>

TranscriptionService::TranscriptionService(QObject* parent)
    : QObject(parent)
    , mWhisper(new QtWhisper::Session(this))
{
    mWhisper->initialize(new QtWhisper::Engine());

    connect(mWhisper, &QtWhisper::Session::statusChanged, this, &TranscriptionService::whisperStatusChanged);

    connect(mWhisper, &QtWhisper::Session::segmentTranscribed, this, &TranscriptionService::transcriptionUpdated);

    connect(mWhisper, &QtWhisper::Session::errorOccurred, this, &TranscriptionService::errorOccurred);

    connect(mWhisper, &QtWhisper::Session::reloadRequired, this, &TranscriptionService::reloadRequired);

    connect(mWhisper, &QtWhisper::Session::isProcessingChanged, this, &TranscriptionService::isProcessingChanged);
}

TranscriptionService::~TranscriptionService() = default;

bool TranscriptionService::isProcessing() const { return mWhisper->isProcessing(); }

QtWhisper::Status TranscriptionService::whisperStatus() const {
    return mWhisper->status();
}

void TranscriptionService::processAudioWindow(const QVector<float>& window) {
    mWhisper->processAudioWindow(window);
}

void TranscriptionService::onActiveSessionChanged(Session* activeSession) {
    mActiveSession = activeSession;
    if (!activeSession) { unloadModels(); return; }
    loadModels(activeSession);
}

void TranscriptionService::onConfigChanged(const SessionConfig& config) {
    mWhisper->setConfig(config.stt);
}

void TranscriptionService::reloadModels() {
    if (!mActiveSession) return;
    loadModels(mActiveSession);
}

void TranscriptionService::loadModels(Session* activeSession) {
    mWhisper->setConfig(activeSession->config().stt);
    mWhisper->loadModel();
}

void TranscriptionService::unloadModels() {
    mWhisper->unloadModel();
}
