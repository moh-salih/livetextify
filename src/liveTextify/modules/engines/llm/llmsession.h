#pragma once
#include <QObject>
#include <QThread>
#include "liveTextify/modules/engines/common.h"


class ILlmEngine;
class LlmSession : public QObject {
    Q_OBJECT
    // Properties are now more descriptive for QML
    Q_PROPERTY(bool isGenerating READ isGenerating NOTIFY isGeneratingChanged)
    Q_PROPERTY(LiveTextify::EngineStatus status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusChanged)

public:
    explicit LlmSession(QObject *parent = nullptr);
    ~LlmSession();

    void initialize(ILlmEngine* engine);

    void setConfig(const LLMConfig& config);

    // Getters
    bool isGenerating() const { return mIsGenerating; }
    LiveTextify::EngineStatus status() const { return mStatus; }
    QString statusText() const;

    Q_INVOKABLE void generate(const QString& prompt, int sessionId);
    Q_INVOKABLE void loadModel();
    Q_INVOKABLE void unloadModel();
    Q_INVOKABLE void stop();

signals:
    void textGenerated(const QString& text, int sessionId);
    void isGeneratingChanged(bool isGenerating);
    void statusChanged(LiveTextify::EngineStatus status);
    void errorOccurred(const QString& msg);

private:
    ILlmEngine                * mEngine = nullptr;
    QThread                   * mWorkerThread = nullptr;
    LiveTextify::EngineStatus   mStatus = LiveTextify::EngineStatus::Idle;
    QSharedPointer<LLMConfig>   mLLMConfig = QSharedPointer<LLMConfig>::create();

    bool                        mIsGenerating = false;
    int                         mCurrentSessionId = -1;
};
