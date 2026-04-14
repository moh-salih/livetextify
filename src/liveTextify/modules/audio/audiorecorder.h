#pragma once
#include <QObject>
#include <QtMultimedia>
#include "liveTextify/interfaces/IAudioSource.h"

// AudioRecorder now implements the IAudioSource interface
class AudioRecorder : public IAudioSource {
    Q_OBJECT
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(QAudioFormat audioFormat READ audioFormat NOTIFY formatChanged)
    Q_PROPERTY(double audioLevel READ audioLevel NOTIFY audioLevelChanged)
public:
    explicit AudioRecorder(QObject *parent = nullptr);

    // Implement IAudioSource interfaces with 'override'
    void setConfig(const AudioConfig& config) override;
    bool start() override;
    void stop() override;
    bool running() const override { return mRunning; }
    double audioLevel() const override { return m_audioLevel; }

    // AudioRecorder specific features
    QAudioFormat audioFormat() const;
    Q_INVOKABLE void setFormat(const QAudioFormat &format);

signals:
    void formatChanged(const QAudioFormat &);

private:
    bool mRunning = false;
    QAudioSource * mAudioSource;
    QIODevice * mIO;
    QAudioFormat mAudioFormat;

    double m_audioLevel = 0.0;
};
