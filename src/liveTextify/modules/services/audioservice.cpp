#include "audioservice.h"
#include "liveTextify/modules/audio/audiorecorder.h"
#include "liveTextify/modules/audio/audiowindowprocessor.h"
#include "liveTextify/modules/engines/common.h"
#include "liveTextify/modules/settings/audioconfigmanager.h"
#include "liveTextify/modules/settings/settingsmanager.h"

#include "liveTextify/core/Logger.h"

AudioService::AudioService(SettingsManager * settings, QObject* parent)
    : QObject(parent), mAudioConfigManager(settings->audioConfigManager()),
    m_audioSource(new AudioRecorder(this)),
    m_windowProcessor(new AudioWindowProcessor(this)){

    m_audioSource->setConfig(mAudioConfigManager->config());;
    m_windowProcessor->setConfig(mAudioConfigManager->config());

    connect(mAudioConfigManager, &AudioConfigManager::configChanged, this, &AudioService::onAudioConfigChanged);

    // Wire Hardware to Processor
    connect(m_audioSource, &IAudioSource::audioChunk, m_windowProcessor, &AudioWindowProcessor::handleChunk);
    connect(m_audioSource, &IAudioSource::stopped,    m_windowProcessor, &AudioWindowProcessor::reset);

    // Expose the processed float array to the outside world
    connect(m_windowProcessor, &AudioWindowProcessor::windowReady, this, &AudioService::windowReady);

    // Bubble up hardware state for the UI
    connect(m_audioSource, &IAudioSource::runningChanged, this, &AudioService::isRecordingChanged);
    connect(m_audioSource, &IAudioSource::audioLevelChanged, this, &AudioService::audioLevelChanged);
}

AudioService::~AudioService() = default;

bool AudioService::isRecording() const {
    return m_audioSource->running();
}

float AudioService::audioLevel() const {
    return static_cast<float>(m_audioSource->audioLevel());
}

void AudioService::startRecording() {
    m_windowProcessor->reset();
    m_audioSource->start();
}

void AudioService::stopRecording() {
    m_audioSource->stop();
}

void AudioService::onAudioConfigChanged(){
    const auto& config = mAudioConfigManager->config();
    Logger::info(QString("AudioService: Max Window: %1 ms, Keep: %2 ms, Step: %3 ms")
        .arg(config.maxWindow.count())
        .arg(config.keep.count())
        .arg(config.step.count())
    );

    m_audioSource->setConfig(config);
    m_windowProcessor->setConfig(config);
}
