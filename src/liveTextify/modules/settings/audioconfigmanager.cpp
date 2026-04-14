#include "liveTextify/modules/settings/audioconfigmanager.h"
#include "liveTextify/core/Logger.h"

#include <QSettings>

AudioConfigManager::AudioConfigManager(QObject * parent): QObject(parent) {}

void AudioConfigManager::setAudioSampleRate(int value){
    if(mConfig.audioSampleRate == value) return;
    mConfig.audioSampleRate = value;
    saveToSettings();
    emit audioSampleRateChanged();
}

void AudioConfigManager::setAudioChannels(int value){
    if(mConfig.audioChannels == value) return;
    mConfig.audioChannels = value;
    saveToSettings();
    emit audioChannelsChanged();
}

void AudioConfigManager::setStep(int value){
    std::chrono::milliseconds valueMS(value);
    if(mConfig.step == valueMS) return;
    mConfig.step = valueMS;
    saveToSettings();
    emit stepChanged();
}

void AudioConfigManager::setKeep(int value){
    std::chrono::milliseconds valueMS(value);
    if(mConfig.keep == valueMS) return;
    mConfig.keep = valueMS;
    saveToSettings();
    emit keepChanged();
}

void AudioConfigManager::setMaxWindow(int value){
    std::chrono::milliseconds valueMS(value);
    if(mConfig.maxWindow == valueMS) return;
    mConfig.maxWindow = valueMS;
    saveToSettings();
    emit maxWindowChanged();
}

void AudioConfigManager::resetToDefaults(){
    mConfig = AudioConfig();
    saveToSettings();

    emit audioSampleRateChanged();
    emit audioChannelsChanged();
    emit stepChanged();
    emit keepChanged();
    emit maxWindowChanged();
    void configChanged();
}

void AudioConfigManager::loadFromSettings(){
    AudioConfig defaultConfig;
    QSettings settings;

    settings.beginGroup("AUDIO");
    mConfig.audioSampleRate = settings.value("audioSampleRate", defaultConfig.audioSampleRate).toInt();
    mConfig.audioChannels   = settings.value("audioChannels", defaultConfig.audioChannels).toInt();

    qint64 keepMs = settings.value("keep", qint64(defaultConfig.keep.count())).toLongLong();
    mConfig.keep = std::chrono::milliseconds(keepMs);

    qint64 stepMS = settings.value("step", qint64(defaultConfig.step.count())).toLongLong();
    mConfig.step = std::chrono::milliseconds(stepMS);

    qint64 maxWindow = settings.value("maxWindow", qint64(defaultConfig.maxWindow.count())).toLongLong();
    mConfig.maxWindow = std::chrono::milliseconds(maxWindow);



    settings.endGroup();

    Logger::info("=== AUDIO CONFIG LOADED ===");
    Logger::info(QString("  Sample Rate: %1 Hz").arg(mConfig.audioSampleRate));
    Logger::info(QString("  Channels: %1").arg(mConfig.audioChannels));
    Logger::info(QString("  Step: %1 ms").arg(mConfig.step.count()));
    Logger::info(QString("  Keep: %1 ms").arg(mConfig.keep.count()));
    Logger::info(QString("  Max Window: %1 ms").arg(mConfig.maxWindow.count()));
}

void AudioConfigManager::saveToSettings(){
    QSettings settings;

    settings.beginGroup("AUDIO");
    settings.setValue("audioSampleRate", mConfig.audioSampleRate);
    settings.setValue("audioChannels", mConfig.audioChannels);
    settings.setValue("keep", qint64(mConfig.keep.count()));
    settings.setValue("step", qint64(mConfig.step.count()));
    settings.setValue("maxWindow", qint64(mConfig.maxWindow.count()));
    settings.endGroup();
    settings.sync();


    Logger::info("=== AUDIO CONFIG SAVED ===");
    Logger::info(QString("  Sample Rate: %1 Hz").arg(mConfig.audioSampleRate));
    Logger::info(QString("  Channels: %1").arg(mConfig.audioChannels));
    Logger::info(QString("  Step: %1 ms").arg(mConfig.step.count()));
    Logger::info(QString("  Keep: %1 ms").arg(mConfig.keep.count()));
    Logger::info(QString("  Max Window: %1 ms").arg(mConfig.maxWindow.count()));


    emit configChanged();
}
