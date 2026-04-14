#pragma once
#include <QObject>
#include <QString>
#include <vector>
#include "liveTextify/modules/engines/common.h"

class ISttEngine : public QObject {
    Q_OBJECT
public:
    explicit ISttEngine(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~ISttEngine() = default;

public slots:
    virtual void setConfig(QSharedPointer<STTConfig> config) = 0;
    virtual void loadModel() = 0;
    virtual void unloadModel() = 0;
    virtual void processWindow(const std::vector<float> &samples) = 0;
    virtual void stop() = 0;
    virtual void reset() = 0;

signals:
    void transcriptionSegmentReady(const QString &text);
    void modelStatusChanged(LiveTextify::EngineStatus status);
    void processingBusyChanged(bool isProcessing);
    void errorEncountered(const QString &message);
};
