#pragma once
#include <QObject>
#include <QString>

#include "liveTextify/modules/engines/common.h"


class ILlmEngine : public QObject {
    Q_OBJECT
public:
    explicit ILlmEngine(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~ILlmEngine() = default;

public slots:
    virtual void setConfig(QSharedPointer<LLMConfig> config) = 0;
    virtual void loadModel() = 0;
    virtual void unloadModel() = 0;

    virtual void generate(const QString &prompt) = 0;
    virtual void stop() = 0;
    virtual void reset() = 0;

signals:
    // Descriptive Signal Naming
    void tokenGenerated(const QString &textChunk);
    void modelStatusChanged(LiveTextify::EngineStatus status);
    void generationBusyChanged(bool isGenerating);
    void errorEncountered(const QString &message);
};
