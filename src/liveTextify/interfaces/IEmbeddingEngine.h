#pragma once
#include <QObject>
#include <QString>
#include <vector>
#include "liveTextify/modules/engines/common.h"


class IEmbeddingEngine : public QObject {
    Q_OBJECT
public:
    explicit IEmbeddingEngine(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~IEmbeddingEngine() = default;

public slots:
    virtual void loadModel() = 0;
    virtual void unloadModel() = 0;
    // We pass text and chunkIndex so the receiver knows exactly what this vector refers to
    virtual void generateEmbedding(const QString &text, int chunkIndex) = 0;
    virtual void setConfig(QSharedPointer<EmbConfig> config) = 0;
    virtual void stop() = 0;
    virtual void reset() = 0;

signals:
    // Descriptive Signal Naming
    void vectorReady(const std::vector<float> &embedding, const QString &text, int chunkIndex);
    void modelStatusChanged(LiveTextify::EngineStatus status);
    void computeBusyChanged(bool isProcessing);
    void errorEncountered(const QString &message);
};
