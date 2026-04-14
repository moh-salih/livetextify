#pragma once
#include "liveTextify/modules/engines/common.h"

class AudioConfigManager: public QObject{
    Q_OBJECT
    Q_PROPERTY(int audioSampleRate READ audioSampleRate WRITE setAudioSampleRate NOTIFY audioSampleRateChanged FINAL)
    Q_PROPERTY(int audioChannels READ audioChannels WRITE setAudioChannels NOTIFY audioChannelsChanged FINAL)
    Q_PROPERTY(int step READ step WRITE setStep NOTIFY stepChanged FINAL)
    Q_PROPERTY(int keep READ keep WRITE setKeep NOTIFY keepChanged FINAL)
    Q_PROPERTY(int maxWindow READ maxWindow WRITE setMaxWindow NOTIFY maxWindowChanged FINAL)
public:
    explicit AudioConfigManager(QObject * parent = nullptr);


    // Getters
    int audioSampleRate()   const { return mConfig.audioSampleRate; }
    int audioChannels()     const { return mConfig.audioChannels; }
    int step()              const { return mConfig.step.count(); }
    int keep()              const { return mConfig.keep.count(); }
    int maxWindow()         const { return mConfig.maxWindow.count(); }


    // Setters
    void setAudioSampleRate(int);
    void setAudioChannels(int);
    void setStep(int);
    void setKeep(int);
    void setMaxWindow(int);

    // state management
    const AudioConfig& config() const { return mConfig; }
    Q_INVOKABLE void resetToDefaults();
    void loadFromSettings();
    void saveToSettings();


signals:
    void audioSampleRateChanged();
    void audioChannelsChanged();
    void stepChanged();
    void keepChanged();
    void maxWindowChanged();
    void configChanged();
private:
    AudioConfig mConfig;
};

