#pragma once
#include <QObject>
#include <QtWhisper/Types.h>

class Session;
struct SessionConfig;

namespace QtWhisper { class Session; }

class TranscriptionService : public QObject {
    Q_OBJECT
public:
    explicit TranscriptionService(QObject* parent = nullptr);
    ~TranscriptionService() override;

    QtWhisper::Status whisperStatus() const;
    bool isProcessing() const;

public slots:
    void processAudioWindow(const QVector<float>& window);
    void onActiveSessionChanged(Session* activeSession);
    void onConfigChanged(const SessionConfig& config);
    void reloadModels();

signals:
    void whisperStatusChanged(QtWhisper::Status status);
    void transcriptionUpdated(const QString& text);
    void reloadRequired();
    void errorOccurred(QtWhisper::Error error);
    void isProcessingChanged(bool processing);

private:
    void loadModels(Session* activeSession);
    void unloadModels();

    QtWhisper::Session* mWhisper;
    Session*            mActiveSession = nullptr;
};
