#pragma once
#include <QObject>
#include <QByteArray>
#include "liveTextify/modules/engines/common.h"

class IAudioSource : public QObject {
    Q_OBJECT
public:
    explicit IAudioSource(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~IAudioSource() = default;

    // Hardware Configuration
    virtual void setConfig(const AudioConfig& config) = 0;

    // Control
    virtual bool start() = 0;
    virtual void stop() = 0;

    // State
    virtual bool running() const = 0;
    virtual double audioLevel() const = 0;

signals:
    void audioChunk(const QByteArray& pcm);
    void runningChanged(bool running);
    void audioLevelChanged();
    void started();
    void stopped();
};
