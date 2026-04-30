#pragma once
#include <QObject>
#include <vector>
#include <memory>
#include <QtWhisper/Types.h>
#include <QtAudioCapture/Types.h>

class Session;

namespace QtAudioCapture { class AudioPipeline; }
namespace QtWhisper      { class Session; }

class TranscriptionService : public QObject {
    Q_OBJECT
public:
    explicit TranscriptionService(QObject* parent = nullptr);
    ~TranscriptionService() override;

    bool isRecording() const;
    QtWhisper::Status whisperStatus() const;

public slots:
    void startTranscription();
    void stopTranscription();
    void onActiveSessionChanged(Session* activeSession);

signals:
    void isRecordingChanged();
    void whisperStatusChanged(QtWhisper::Status status);
    void transcriptionUpdated(const QString& text);

    // Typed error signals — consumed by SessionManager
    void errorOccurred(QtWhisper::Error error);
    void audioErrorOccurred(QtAudioCapture::Error error);

private:
    void loadModels(Session* activeSession);
    void unloadModels();

    QtAudioCapture::AudioPipeline * mAudioPipeline;
    QtWhisper::Session            * mWhisper;
};
