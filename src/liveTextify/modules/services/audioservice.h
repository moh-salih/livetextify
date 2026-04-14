#pragma once
#include <QObject>
#include <vector>

class IAudioSource;
class AudioWindowProcessor;
class SettingsManager;
class AudioConfigManager;


class AudioService : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isRecording READ isRecording NOTIFY isRecordingChanged)
    Q_PROPERTY(float audioLevel READ audioLevel NOTIFY audioLevelChanged)

public:
    explicit AudioService(SettingsManager * settings, QObject* parent = nullptr);
    ~AudioService() override;

    bool isRecording() const;
    float audioLevel() const;

public slots:
    void startRecording();
    void stopRecording();
private slots:
    void onAudioConfigChanged();
signals:
    void isRecordingChanged();
    void audioLevelChanged();

    void windowReady(const std::vector<float>& samples);

private:
    IAudioSource * m_audioSource;
    AudioWindowProcessor* m_windowProcessor;
    AudioConfigManager * mAudioConfigManager;
};
