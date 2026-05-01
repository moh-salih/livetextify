// transcriptionservice.cpp
#include "transcriptionservice.h"
#include "liveTextify/modules/session/session.h"
#include "liveTextify/core/Logger.h"

#include <QtAudioCapture/AudioPipeline.h>
#include <QtWhisper/Session.h>
#include <QtWhisper/Engine.h>

TranscriptionService::TranscriptionService(QObject* parent)
    : QObject(parent)
    , mAudioPipeline(new QtAudioCapture::AudioPipeline(this))
    , mWhisper(new QtWhisper::Session(this))
{
    mWhisper->initialize(new QtWhisper::Engine());

    connect(mAudioPipeline, &QtAudioCapture::AudioPipeline::windowReady,
            mWhisper, &QtWhisper::Session::processAudioWindow);

    connect(mAudioPipeline, &QtAudioCapture::AudioPipeline::statusChanged,
            this, &TranscriptionService::isRecordingChanged);

    connect(mWhisper, &QtWhisper::Session::statusChanged,
            this, &TranscriptionService::whisperStatusChanged);

    connect(mWhisper, &QtWhisper::Session::segmentTranscribed,
            this, &TranscriptionService::transcriptionUpdated);

    // Forward typed errors up to SessionManager
    connect(mWhisper, &QtWhisper::Session::errorOccurred,
            this, &TranscriptionService::errorOccurred);

    connect(mAudioPipeline, &QtAudioCapture::AudioPipeline::errorOccurred,
            this, &TranscriptionService::audioErrorOccurred);

    connect(mWhisper, &QtWhisper::Session::reloadRequired,this, []{
        Logger::info("Whisper is saying relad is required");
    });

}

void TranscriptionService::onActiveSessionConfigChanged(const SessionConfig& config) {
    mWhisper->setConfig(config.stt);
    mAudioPipeline->setConfig(config.audio);
    // No loadModel() call here — Engine::setConfig handles reload
    // automatically via autoReload flag
}

TranscriptionService::~TranscriptionService() = default;

bool TranscriptionService::isRecording() const {
    return mAudioPipeline->isRunning();
}

QtWhisper::Status TranscriptionService::whisperStatus() const {
    return mWhisper->status();
}

void TranscriptionService::startTranscription() {
    mAudioPipeline->start();
}

void TranscriptionService::stopTranscription() {
    mAudioPipeline->stop();
}

void TranscriptionService::onActiveSessionChanged(Session* activeSession) {
    if (!activeSession) {
        unloadModels();
        return;
    }
    Logger::info("[TranscriptionService] active session changed, model: "
                 + activeSession->config().stt.modelPath);
    loadModels(activeSession);
}


void TranscriptionService::loadModels(Session* activeSession) {
    const auto& config = activeSession->config();
    mWhisper->setConfig(config.stt);
    mAudioPipeline->setConfig(config.audio);
}

void TranscriptionService::unloadModels() {
    if (isRecording()) stopTranscription();
    mWhisper->unloadModel();
}
