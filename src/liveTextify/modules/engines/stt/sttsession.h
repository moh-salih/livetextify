#pragma once

#include <QObject>
#include <QThread>
#include <QString>
#include "liveTextify/interfaces/ISttEngine.h"
#include "liveTextify/modules/engines/common.h"

class SttSession : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString transcription READ transcription NOTIFY transcriptionChanged)
    Q_PROPERTY(LiveTextify::EngineStatus status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusChanged)

public:
    using Status = LiveTextify::EngineStatus;

    explicit SttSession(QObject *parent = nullptr);
    ~SttSession();

    void initialize(ISttEngine* engine);
    void setConfig(const STTConfig& sttConfig);
    void loadModel();
    void unloadModel();

    QString transcription() const { return mTranscription; }
    LiveTextify::EngineStatus status() const { return mStatus; }
    QString statusText() const;

    Q_INVOKABLE void clear();

public slots:
    void processAudioWindow(const std::vector<float> &samples);
    void startInference();
    void stopInference();

signals:
    void transcriptionChanged(const QString& text);
    void statusChanged();
    void partialResult(const QString& text);
    void errorOccurred(const QString& msg);

private slots:
    void onPartialResult(const QString& text);

private:
    ISttEngine                  * mEngine       = nullptr;
    QThread                     * mWorkerThread = nullptr;
    LiveTextify::EngineStatus     mStatus       = LiveTextify::EngineStatus::Idle;
    QSharedPointer<STTConfig>     mSTTConfig    = QSharedPointer<STTConfig>::create();

    QString                       mTranscription;
};
