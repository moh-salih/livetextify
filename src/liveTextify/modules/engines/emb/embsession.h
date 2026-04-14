#pragma once
#include <QObject>
#include <QThread>
#include <vector>
#include "liveTextify/modules/engines/common.h"



class IEmbeddingEngine;
class EmbSession : public QObject {
    Q_OBJECT
    Q_PROPERTY(LiveTextify::EngineStatus status READ status         NOTIFY statusChanged)
    Q_PROPERTY(bool isGenerating                READ isGenerating   NOTIFY isGeneratingChanged)
    Q_PROPERTY(QString statusText               READ statusText     NOTIFY statusChanged)

public:
    explicit EmbSession(QObject *parent = nullptr);
    ~EmbSession();

    void initialize(IEmbeddingEngine * engine);


    void setConfig(const EmbConfig& config);

    // Logic API
    void loadModel();
    void unloadModel();
    void generateEmbedding(const QString& text, int chunkIndex);
    Q_INVOKABLE void stop();

    // Getters
    LiveTextify::EngineStatus status() const { return mStatus; }
    bool isGenerating() const { return mIsGenerating; }
    QString statusText() const;

signals:
    void embeddingReady(const std::vector<float>& embedding, const QString& text, int chunkIndex);
    void statusChanged();
    void isGeneratingChanged(bool);
    void errorOccurred(const QString& msg);

private:
    IEmbeddingEngine            * mEngine = nullptr;
    QThread                     * mWorkerThread = nullptr;
    QSharedPointer<EmbConfig>     mEmbConfig = QSharedPointer<EmbConfig>::create();
    LiveTextify::EngineStatus     mStatus = LiveTextify::EngineStatus::Idle;
    bool                          mIsGenerating = false;
};
