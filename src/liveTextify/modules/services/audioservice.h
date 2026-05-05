#pragma once
#include <QObject>
#include <QtAudioCapture/Types.h>

class Session;
struct SessionConfig;

namespace QtAudioCapture { class AudioPipeline; }

class AudioService : public QObject {
    Q_OBJECT
public:
    explicit AudioService(QObject* parent = nullptr);
    ~AudioService() override;

    bool isRecording() const;

public slots:
    void start();
    void stop();
    void onActiveSessionChanged(Session* activeSession);
    void onConfigChanged(const SessionConfig& config);

signals:
    void isRecordingChanged();
    void audioWindowReady(const QVector<float>& window);
    void audioErrorOccurred(QtAudioCapture::Error error);

private:
    QtAudioCapture::AudioPipeline* mAudioPipeline;
    Session*                       mActiveSession = nullptr;
};
