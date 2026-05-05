#include "liveTextify/modules/services/audioservice.h"
#include "liveTextify/modules/session/session.h"
#include "liveTextify/core/Logger.h"

#include <QtAudioCapture/AudioPipeline.h>

AudioService::AudioService(QObject* parent): QObject(parent), mAudioPipeline(new QtAudioCapture::AudioPipeline(this))
{
    connect(mAudioPipeline, &QtAudioCapture::AudioPipeline::statusChanged, this, &AudioService::isRecordingChanged);

    connect(mAudioPipeline, &QtAudioCapture::AudioPipeline::errorOccurred, this, &AudioService::audioErrorOccurred);

    connect(mAudioPipeline, &QtAudioCapture::AudioPipeline::windowReady, this, &AudioService::audioWindowReady);
}

AudioService::~AudioService() = default;

bool AudioService::isRecording() const {
    return mAudioPipeline->isRunning();
}

void AudioService::start() {
    mAudioPipeline->start();
}

void AudioService::stop() {
    mAudioPipeline->stop();
}

void AudioService::onActiveSessionChanged(Session* activeSession) {
    mActiveSession = activeSession;

    if (!activeSession) {
        if (isRecording()) stop();
        return;
    }

    mAudioPipeline->setConfig(activeSession->config().audio);
}

void AudioService::onConfigChanged(const SessionConfig& config) {
    mAudioPipeline->setConfig(config.audio);
}
