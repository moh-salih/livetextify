#include "liveTextify/modules/settings/audioconfigmanager.h"
#include <QSettings>

AudioConfigManager::AudioConfigManager(QObject * parent): QObject(parent) {}

void AudioConfigManager::setStep(int value) {
    if (mConfig.stepDurationMs == static_cast<int>(value)) return;
    mConfig.stepDurationMs = value;

    emit stepChanged();
}

void AudioConfigManager::setKeep(int value) {
    // keep = window - step, so setting keep means adjusting step
    const int newStep = mConfig.windowDurationMs - value;
    if (mConfig.stepDurationMs == newStep) return;
    mConfig.stepDurationMs = newStep;

    emit keepChanged();
    emit stepChanged(); // step changes when keep changes
}

void AudioConfigManager::setMaxWindow(int value) {
    if (mConfig.windowDurationMs == value) return;
    mConfig.windowDurationMs = value;

    emit maxWindowChanged();
    // keep is derived from window, so it changes too
    emit keepChanged();
}


void AudioConfigManager::setAudioSampleRate(int value){
    if(mConfig.targetSampleRate == value) return;
    mConfig.targetSampleRate = value;

    emit audioSampleRateChanged();
}

void AudioConfigManager::setAudioChannels(int value){
    if(mConfig.channelCount == value) return;
    mConfig.channelCount = value;

    emit audioChannelsChanged();
}

void AudioConfigManager::resetToDefaults() {
    mConfig = QtAudioCapture::Config();

    emit audioSampleRateChanged();
    emit audioChannelsChanged();
    emit stepChanged();
    emit keepChanged();
    emit maxWindowChanged();
    emit configChanged();
}

void AudioConfigManager::loadFromSettings() {
    QtAudioCapture::Config defaultConfig;
    QSettings settings;

    settings.beginGroup("AUDIO");
    mConfig.targetSampleRate = settings.value("audioSampleRate", defaultConfig.targetSampleRate).toInt();
    mConfig.channelCount     = settings.value("audioChannels", defaultConfig.channelCount).toInt();
    mConfig.windowDurationMs = settings.value("maxWindow", defaultConfig.windowDurationMs).toInt();
    mConfig.stepDurationMs   = settings.value("step", defaultConfig.stepDurationMs).toInt();
    settings.endGroup();

}

void AudioConfigManager::saveToSettings() {
    QSettings settings;

    settings.beginGroup("AUDIO");
    settings.setValue("audioSampleRate", mConfig.targetSampleRate);
    settings.setValue("audioChannels",   mConfig.channelCount);
    settings.setValue("maxWindow",       mConfig.windowDurationMs);
    settings.setValue("step",            mConfig.stepDurationMs);
    settings.endGroup();
    settings.sync();
}
