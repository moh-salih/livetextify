#pragma once

#include <QObject>
#include "liveTextify/modules/session/session.h" // for SessionConfig

// SessionSettings holds the global defaults that are applied to every new
// session and persisted to QSettings immediately on change. Per-session
// overrides are applied on top via seedFrom() when a session is activated.
//
// Properties that affect an already-loaded model (e.g. threadCount, modelPath)
// do NOT live here as Q_PROPERTYs — the reload signal comes from QtWhisper /
// QtLlama and is forwarded to reloadRequired() by SessionManager.

class SessionSettings : public QObject {
    Q_OBJECT

    // ── STT ───────────────────────────────────────────────────────────────────
    Q_PROPERTY(QString language           READ language           WRITE setLanguage                 NOTIFY languageChanged           FINAL)
    Q_PROPERTY(bool    autoDetectLanguage READ autoDetectLanguage WRITE setAutoDetectLanguage       NOTIFY autoDetectLanguageChanged FINAL)
    Q_PROPERTY(bool    translate          READ translate          WRITE setTranslate                NOTIFY translateChanged          FINAL)

    // ── LLM ───────────────────────────────────────────────────────────────────
    Q_PROPERTY(QString systemPrompt         READ systemPrompt          WRITE setSystemPrompt        NOTIFY systemPromptChanged      FINAL)
    Q_PROPERTY(float   temperature          READ temperature           WRITE setTemperature         NOTIFY temperatureChanged       FINAL)
    Q_PROPERTY(float   topP                 READ topP                  WRITE setTopP                NOTIFY topPChanged              FINAL)
    Q_PROPERTY(int     topK                 READ topK                  WRITE setTopK                NOTIFY topKChanged              FINAL)
    Q_PROPERTY(float   repeatPenalty        READ repeatPenalty         WRITE setRepeatPenalty       NOTIFY repeatPenaltyChanged     FINAL)
    Q_PROPERTY(int     maxTokens            READ maxTokens             WRITE setMaxTokens           NOTIFY maxTokensChanged         FINAL)
    Q_PROPERTY(int llmContextLength         READ llmContextLength      WRITE setLlmContextLength    NOTIFY llmContextLengthChanged  FINAL)
    Q_PROPERTY(int resolvedContextLength    READ resolvedContextLength NOTIFY resolvedContextLengthChanged                          FINAL)

    // ── RAG ───────────────────────────────────────────────────────────────────
    Q_PROPERTY(bool  enableRag    READ enableRag    WRITE setEnableRag    NOTIFY enableRagChanged    FINAL)
    Q_PROPERTY(int   ragTopK      READ ragTopK      WRITE setRagTopK      NOTIFY ragTopKChanged      FINAL)
    Q_PROPERTY(float ragMinScore  READ ragMinScore  WRITE setRagMinScore  NOTIFY ragMinScoreChanged  FINAL)
    Q_PROPERTY(int   minChunkSize READ minChunkSize WRITE setMinChunkSize NOTIFY minChunkSizeChanged FINAL)
    Q_PROPERTY(int   maxChunkSize READ maxChunkSize WRITE setMaxChunkSize NOTIFY maxChunkSizeChanged FINAL)

    // ── Audio ─────────────────────────────────────────────────────────────────
    Q_PROPERTY(int step      READ step      WRITE setStep      NOTIFY stepChanged      FINAL)
    Q_PROPERTY(int keep      READ keep      WRITE setKeep      NOTIFY keepChanged      FINAL)
    Q_PROPERTY(int maxWindow READ maxWindow WRITE setMaxWindow NOTIFY maxWindowChanged FINAL)

public:
    explicit SessionSettings(QObject* parent = nullptr);

    // ── Lifecycle ─────────────────────────────────────────────────────────────
    void load(); // call once at startup

    // ── Bridge ────────────────────────────────────────────────────────────────
    void          seedFrom(const SessionConfig& cfg); // copies user-visible fields in
    SessionConfig toConfig() const;                   // full struct for services

    // ── Model path (owned by ModelManager, not a Q_PROPERTY) ─────────────────
    void setModelPath(const QString& type, const QString& path);

    // ── STT getters ───────────────────────────────────────────────────────────
    QString language()           const { return mConfig.stt.language; }
    bool    autoDetectLanguage() const { return mConfig.stt.autoDetectLanguage; }
    bool    translate()          const { return mConfig.stt.translate; }

    // ── LLM getters ───────────────────────────────────────────────────────────
    QString systemPrompt()          const { return mConfig.systemPrompt; }
    float   temperature()           const { return mConfig.llm.temperature; }
    float   topP()                  const { return mConfig.llm.topP; }
    int     topK()                  const { return mConfig.llm.topK; }
    float   repeatPenalty()         const { return mConfig.llm.repeatPenalty; }
    int     maxTokens()             const { return mConfig.llm.maxTokens; }
    int     resolvedContextLength() const { return mResolvedContextLength; }
    int     llmContextLength()      const { return mConfig.llm.contextLength; }
    // ── RAG getters ───────────────────────────────────────────────────────────
    bool  enableRag()    const { return mConfig.enableRag; }
    int   ragTopK()      const { return mConfig.rag.search.topK; }
    float ragMinScore()  const { return mConfig.rag.search.minSimilarity; }
    int   minChunkSize() const { return mConfig.rag.chunking.minChunkLength; }
    int   maxChunkSize() const { return mConfig.rag.chunking.maxChunkLength; }

    // ── Audio getters ─────────────────────────────────────────────────────────
    int step()      const { return mConfig.audio.stepDurationMs; }
    int keep()      const { return mConfig.audio.windowDurationMs - mConfig.audio.stepDurationMs; }
    int maxWindow() const { return mConfig.audio.windowDurationMs; }

    // ── STT setters ───────────────────────────────────────────────────────────
    void setLanguage(const QString& v);
    void setAutoDetectLanguage(bool v);
    void setTranslate(bool v);

    // ── LLM setters ───────────────────────────────────────────────────────────
    void setSystemPrompt(const QString& v);
    void setTemperature(float v);
    void setTopP(float v);
    void setTopK(int v);
    void setRepeatPenalty(float v);
    void setMaxTokens(int v);
    void onContextLengthResolved(int length);
    void setLlmContextLength(int v);
    // ── RAG setters ───────────────────────────────────────────────────────────
    void setEnableRag(bool v);
    void setRagTopK(int v);
    void setRagMinScore(float v);
    void setMinChunkSize(int v);
    void setMaxChunkSize(int v);

    // ── Audio setters ─────────────────────────────────────────────────────────
    void setStep(int v);
    void setKeep(int v);
    void setMaxWindow(int v);

    // ── QML callables ─────────────────────────────────────────────────────────
    Q_INVOKABLE void approveReload();
    Q_INVOKABLE void resetToDefaults();

signals:
    // Per-property signals
    void languageChanged();
    void autoDetectLanguageChanged();
    void translateChanged();

    void systemPromptChanged();
    void temperatureChanged();
    void topPChanged();
    void topKChanged();
    void repeatPenaltyChanged();
    void maxTokensChanged();
    void resolvedContextLengthChanged();
    void llmContextLengthChanged();

    void enableRagChanged();
    void ragTopKChanged();
    void ragMinScoreChanged();
    void minChunkSizeChanged();
    void maxChunkSizeChanged();

    void stepChanged();
    void keepChanged();
    void maxWindowChanged();

    // Aggregate — SessionService uses this to update the active session
    void configChanged(const SessionConfig& cfg);

    // Emitted by SessionManager when QtWhisper / QtLlama request a model reload.
    // QML listens to prompt the user; on confirm, QML calls SessionManager::reloadModels().
    void reloadRequired();
    void reloadApproved();


private:
    void save();         // persists mConfig to QSettings — always called by notifyChange()
    void notifyChange(); // save() + emit configChanged()

    SessionConfig mConfig; // single source of truth for all settings
    int mResolvedContextLength = 0;
};
