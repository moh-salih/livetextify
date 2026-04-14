#pragma once
#include <QObject>

class SttSession;
class LlmSession;
class EmbSession;
class Session;
class SettingsManager;
class STTConfigManager;
class LLMConfigManager;
class EmbConfigManager;

class AiService : public QObject {
    Q_OBJECT
    Q_PROPERTY(SttSession* sttSession READ sttSession CONSTANT)
    Q_PROPERTY(LlmSession* llmSession READ llmSession CONSTANT)
    Q_PROPERTY(EmbSession* embSession READ embSession CONSTANT)

public:
    explicit AiService(SettingsManager * settings, QObject* parent = nullptr);
    ~AiService();

    SttSession* sttSession() const { return mSttSession; }
    LlmSession* llmSession() const { return mLlmSession; }
    EmbSession* embSession() const { return mEmbSession; }


public slots:
    void onActiveSessionChanged(Session* activeSession);
    void processAudioWindow(const std::vector<float>& samples);
    void unloadModels();
    void loadModels(Session* activeSession);

private:
    SttSession          * mSttSession;
    LlmSession          * mLlmSession;
    EmbSession          * mEmbSession;

    STTConfigManager    * mSTTConfigManager;
    LLMConfigManager    * mLLMConfigManager;
    EmbConfigManager    * mEmbConfigManager;
};
