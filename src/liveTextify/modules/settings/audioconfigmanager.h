#pragma once
#include <QObject>
#include <QtAudioCapture/Types.h>

class AudioConfigManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(int audioSampleRate READ audioSampleRate WRITE setAudioSampleRate NOTIFY audioSampleRateChanged FINAL)
    Q_PROPERTY(int audioChannels   READ audioChannels   WRITE setAudioChannels   NOTIFY audioChannelsChanged   FINAL)
    Q_PROPERTY(int step            READ step            WRITE setStep            NOTIFY stepChanged            FINAL)
    Q_PROPERTY(int keep            READ keep            WRITE setKeep            NOTIFY keepChanged            FINAL)
    Q_PROPERTY(int maxWindow       READ maxWindow       WRITE setMaxWindow       NOTIFY maxWindowChanged       FINAL)
public:
    explicit AudioConfigManager(QObject *parent = nullptr);

    int audioSampleRate() const { return mConfig.targetSampleRate; }
    int audioChannels()   const { return mConfig.channelCount; }
    int step()            const { return mConfig.stepDurationMs; }
    int keep()            const { return mConfig.windowDurationMs - mConfig.stepDurationMs; }
    int maxWindow()       const { return mConfig.windowDurationMs; }

    void setAudioSampleRate(int value);
    void setAudioChannels(int value);
    void setStep(int value);
    void setKeep(int value);
    void setMaxWindow(int value);

    const QtAudioCapture::Config &config() const { return mConfig; }
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
    QtAudioCapture::Config mConfig;
};
