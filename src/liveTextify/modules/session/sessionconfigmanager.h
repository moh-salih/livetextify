// sessionconfigmanager.h
#pragma once
#include <QObject>
#include "liveTextify/types.h"
#include "liveTextify/modules/session/session.h" // for SessionConfig

class SessionConfigManager : public QObject {
    Q_OBJECT

    // STT
    Q_PROPERTY(QString sttModelPath        READ sttModelPath        WRITE setSttModelPath        NOTIFY sttModelPathChanged        FINAL)
    Q_PROPERTY(QString language            READ language            WRITE setLanguage            NOTIFY languageChanged            FINAL)
    Q_PROPERTY(bool    autoDetectLanguage  READ autoDetectLanguage  WRITE setAutoDetectLanguage  NOTIFY autoDetectLanguageChanged  FINAL)
    Q_PROPERTY(bool    translate           READ translate           WRITE setTranslate           NOTIFY translateChanged           FINAL)

    // LLM
    Q_PROPERTY(QString llmModelPath   READ llmModelPath   WRITE setLlmModelPath   NOTIFY llmModelPathChanged   FINAL)
    Q_PROPERTY(QString systemPrompt   READ systemPrompt   WRITE setSystemPrompt   NOTIFY systemPromptChanged   FINAL)
    Q_PROPERTY(float   temperature    READ temperature    WRITE setTemperature    NOTIFY temperatureChanged    FINAL)
    Q_PROPERTY(float   topP           READ topP           WRITE setTopP           NOTIFY topPChanged           FINAL)
    Q_PROPERTY(int     topK           READ topK           WRITE setTopK           NOTIFY topKChanged           FINAL)
    Q_PROPERTY(float   repeatPenalty  READ repeatPenalty  WRITE setRepeatPenalty  NOTIFY repeatPenaltyChanged  FINAL)
    Q_PROPERTY(int     maxTokens      READ maxTokens      WRITE setMaxTokens      NOTIFY maxTokensChanged      FINAL)
    Q_PROPERTY(int     contextLength  READ contextLength  WRITE setContextLength  NOTIFY contextLengthChanged  FINAL)
    Q_PROPERTY(int     threadCount    READ threadCount    WRITE setThreadCount    NOTIFY threadCountChanged    FINAL)
    Q_PROPERTY(int     batchSize      READ batchSize      WRITE setBatchSize      NOTIFY batchSizeChanged      FINAL)

    // Embedder
    Q_PROPERTY(QString embModelPath READ embModelPath WRITE setEmbModelPath NOTIFY embModelPathChanged FINAL)

    // RAG
    Q_PROPERTY(bool  enableRag        READ enableRag        WRITE setEnableRag        NOTIFY enableRagChanged        FINAL)
    Q_PROPERTY(int   ragTopK          READ ragTopK          WRITE setRagTopK          NOTIFY ragTopKChanged          FINAL)
    Q_PROPERTY(float ragMinScore      READ ragMinScore      WRITE setRagMinScore      NOTIFY ragMinScoreChanged      FINAL)
    Q_PROPERTY(int   minChunkSize     READ minChunkSize     WRITE setMinChunkSize     NOTIFY minChunkSizeChanged     FINAL)
    Q_PROPERTY(int   maxChunkSize     READ maxChunkSize     WRITE setMaxChunkSize     NOTIFY maxChunkSizeChanged     FINAL)

public:
    explicit SessionConfigManager(QObject* parent = nullptr);

    // Called by SessionService when active session changes
    void seedFrom(const SessionConfig& cfg);
    void clear();
    SessionConfig toConfig() const { return mConfig; }

    // STT getters
    QString sttModelPath()       const { return mConfig.stt.modelPath; }
    QString language()           const { return mConfig.stt.language; }
    bool    autoDetectLanguage() const { return mConfig.stt.autoDetectLanguage; }
    bool    translate()          const { return mConfig.stt.translate; }

    // LLM getters
    QString llmModelPath()  const { return mConfig.llm.modelPath; }
    QString systemPrompt()  const { return mConfig.systemPrompt; }
    float   temperature()   const { return mConfig.llm.temperature; }
    float   topP()          const { return mConfig.llm.topP; }
    int     topK()          const { return mConfig.llm.topK; }
    float   repeatPenalty() const { return mConfig.llm.repeatPenalty; }
    int     maxTokens()     const { return mConfig.llm.maxTokens; }
    int     contextLength() const { return mConfig.llm.contextLength; }
    int     threadCount()   const { return mConfig.llm.threadCount; }
    int     batchSize()     const { return mConfig.llm.batchSize; }

    // Embedder getters
    QString embModelPath() const { return mConfig.emb.modelPath; }

    // RAG getters
    bool  enableRag()    const { return mConfig.enableRag; }
    int   ragTopK()      const { return mConfig.rag.search.topK; }
    float ragMinScore()  const { return mConfig.rag.search.minSimilarity; }
    int   minChunkSize() const { return mConfig.rag.chunking.minChunkLength; }
    int   maxChunkSize() const { return mConfig.rag.chunking.maxChunkLength; }

    // STT setters
    void setSttModelPath(const QString& v);
    void setLanguage(const QString& v);
    void setAutoDetectLanguage(bool v);
    void setTranslate(bool v);

    // LLM setters
    void setLlmModelPath(const QString& v);
    void setSystemPrompt(const QString& v);
    void setTemperature(float v);
    void setTopP(float v);
    void setTopK(int v);
    void setRepeatPenalty(float v);
    void setMaxTokens(int v);
    void setContextLength(int v);
    void setThreadCount(int v);
    void setBatchSize(int v);

    // Embedder setters
    void setEmbModelPath(const QString& v);

    // RAG setters
    void setEnableRag(bool v);
    void setRagTopK(int v);
    void setRagMinScore(float v);
    void setMinChunkSize(int v);
    void setMaxChunkSize(int v);

signals:
    // Per-property signals
    void sttModelPathChanged();
    void languageChanged();
    void autoDetectLanguageChanged();
    void translateChanged();

    void llmModelPathChanged();
    void systemPromptChanged();
    void temperatureChanged();
    void topPChanged();
    void topKChanged();
    void repeatPenaltyChanged();
    void maxTokensChanged();
    void contextLengthChanged();
    void threadCountChanged();
    void batchSizeChanged();

    void embModelPathChanged();

    void enableRagChanged();
    void ragTopKChanged();
    void ragMinScoreChanged();
    void minChunkSizeChanged();
    void maxChunkSizeChanged();

    // Aggregate — SessionService connects to this to write back to session
    void configChanged();

    // Emitted by seedFrom() so QML rebinds everything at once
    void sessionSwitched();

private:
    SessionConfig mConfig;
};
