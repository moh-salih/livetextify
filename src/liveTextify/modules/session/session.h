#pragma once
#include <QtCore>
#include "liveTextify/modules/chat/conversation.h"
#include "liveTextify/modules/engines/common.h"


struct SessionConfig {
    QString llmModelPath;
    QString sttModelPath;
    QString embModelPath;

    QString systemPrompt        = "You are a helpful assistant";
    int     contextLength       = 4096;
    int     maxTokens           = -1;
    float   temperature         = 0.7f;

    QString language            = "en";
    bool    autoDetectLanguage  = false;
    bool    translate           = false;

    bool    enableRag           = true;
};

struct SessionRecord {
    int           dbId      = -1;
    QString       title;
    QDateTime     createdAt;
    SessionConfig config;
};



class Session : public QObject {
    Q_OBJECT
    Q_PROPERTY(int          sessionID       READ sessionID      CONSTANT)
    Q_PROPERTY(QDateTime    createdAt       READ createdAt      WRITE setCreatedAt      NOTIFY createdAtChanged FINAL)
    Q_PROPERTY(QString      title           READ title          WRITE setTitle          NOTIFY titleChanged)
    Q_PROPERTY(QString      transcription   READ transcription  WRITE setTranscription  NOTIFY transcriptionChanged)
    Q_PROPERTY(Conversation* conversation   READ conversation   CONSTANT)

    // Model paths
    Q_PROPERTY(QString  llmModelPath    READ llmModelPath   WRITE setLlmModelPath   NOTIFY configChanged)
    Q_PROPERTY(QString  sttModelPath    READ sttModelPath   WRITE setSttModelPath   NOTIFY configChanged)
    Q_PROPERTY(QString  embModelPath    READ embModelPath   WRITE setEmbModelPath   NOTIFY configChanged)

    // LLM per-session config
    Q_PROPERTY(QString  systemPrompt    READ systemPrompt   WRITE setSystemPrompt   NOTIFY configChanged)
    Q_PROPERTY(int      contextLength   READ contextLength  WRITE setContextLength  NOTIFY configChanged)
    Q_PROPERTY(int      maxTokens       READ maxTokens      WRITE setMaxTokens      NOTIFY configChanged)
    Q_PROPERTY(float    temperature     READ temperature    WRITE setTemperature    NOTIFY configChanged)

    // STT per-session config
    Q_PROPERTY(QString  language            READ language           WRITE setLanguage           NOTIFY configChanged)
    Q_PROPERTY(bool     autoDetectLanguage  READ autoDetectLanguage WRITE setAutoDetectLanguage NOTIFY configChanged)
    Q_PROPERTY(bool     translate           READ translate          WRITE setTranslate          NOTIFY configChanged)

    // RAG
    Q_PROPERTY(bool     enableRag       READ enableRag      WRITE setEnableRag      NOTIFY configChanged)

public:
    explicit Session(QObject *parent = nullptr);
    ~Session();

    // Identity
    int       sessionID() const { return mDbId; }
    QString   title()     const { return mTitle; }
    QDateTime createdAt() const { return mCreatedAt; }

    void setDbId(int id)                   { mDbId = id; }
    void setTitle(const QString& t)        { mTitle = t; emit titleChanged(); }
    void setCreatedAt(const QDateTime& dt) { mCreatedAt = dt; }

    // Config access — for AiService merge and SessionRepository
    const SessionConfig& config() const { return mConfig; }
    SessionConfig&       config()       { return mConfig; }

    // Model paths
    QString llmModelPath() const { return mConfig.llmModelPath; }
    QString sttModelPath() const { return mConfig.sttModelPath; }
    QString embModelPath() const { return mConfig.embModelPath; }

    void setLlmModelPath(const QString& v) { mConfig.llmModelPath = v; emit configChanged(); }
    void setSttModelPath(const QString& v) { mConfig.sttModelPath = v; emit configChanged(); }
    void setEmbModelPath(const QString& v) { mConfig.embModelPath = v; emit configChanged(); }

    // LLM
    QString systemPrompt()  const { return mConfig.systemPrompt; }
    int     contextLength() const { return mConfig.contextLength; }
    int     maxTokens()     const { return mConfig.maxTokens; }
    float   temperature()   const { return mConfig.temperature; }

    void setSystemPrompt(const QString& v) { mConfig.systemPrompt  = v; emit configChanged(); }
    void setContextLength(int v)           { mConfig.contextLength = v; emit configChanged(); }
    void setMaxTokens(int v)               { mConfig.maxTokens     = v; emit configChanged(); }
    void setTemperature(float v)           { mConfig.temperature   = v; emit configChanged(); }

    // STT
    QString language()           const { return mConfig.language; }
    bool    autoDetectLanguage() const { return mConfig.autoDetectLanguage; }
    bool    translate()          const { return mConfig.translate; }

    void setLanguage(const QString& v)     { mConfig.language           = v; emit configChanged(); }
    void setAutoDetectLanguage(bool v)     { mConfig.autoDetectLanguage = v; emit configChanged(); }
    void setTranslate(bool v)              { mConfig.translate          = v; emit configChanged(); }

    // RAG
    bool enableRag() const             { return mConfig.enableRag; }
    void setEnableRag(bool v)          { mConfig.enableRag = v; emit configChanged(); }

    // Transcription
    QString transcription() const { return mTranscription; }
    void setTranscription(const QString& t);

    Conversation* conversation() const { return mConversation; }

    Q_INVOKABLE void sendMessage(const QString& text);

signals:
    void configChanged();
    void titleChanged();
    void transcriptionChanged();
    void userMessageReady(const QString& text, int sessionId);
    void createdAtChanged();
private:
    int           mDbId     = -1;
    QString       mTitle;
    QDateTime     mCreatedAt = QDateTime::currentDateTime();

    SessionConfig mConfig;

    Conversation* mConversation;
    QString       mTranscription;
};
