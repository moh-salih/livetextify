// session.h
#pragma once
#include <QObject>
#include <QDateTime>
#include "liveTextify/modules/chat/conversation.h"
#include "liveTextify/types.h"
#include <QtLlama/Types.h>
#include <QtWhisper/Types.h>

struct SessionConfig {
    SttConfig   stt;
    LLMConfig   llm;
    EmbConfig   emb;
    RagConfig   rag;
    AudioConfig audio;
    QString     systemPrompt;
    bool        enableRag = true;
};

class Session : public QObject {
    Q_OBJECT

    Q_PROPERTY(int            sessionID     READ sessionID     CONSTANT)
    Q_PROPERTY(QString        title         READ title         WRITE setTitle  NOTIFY titleChanged)
    Q_PROPERTY(QDateTime      createdAt     READ createdAt                     NOTIFY createdAtChanged)

    // Heavy data — null until session is activated
    Q_PROPERTY(Conversation*  conversation  READ conversation  NOTIFY conversationLoaded)
    Q_PROPERTY(QString        transcription READ transcription NOTIFY transcriptionChanged)

public:

    explicit Session(QObject* parent = nullptr);
    ~Session() = default;

    // Identity
    int              sessionID()     const { return mDbId; }
    QString          title()         const { return mTitle; }
    QDateTime        createdAt()     const { return mCreatedAt; }
    QString          transcription() const { return mTranscription; }
    Conversation*    conversation()  const { return mConversation; }

    // Config access — for services only, not exposed to QML
    const SessionConfig& config() const { return mConfig; }
    SessionConfig&       config()       { return mConfig; }

    // Called by SessionService only
    void setDbId(int id)                        { mDbId = id; }
    void setTitle(const QString& t)             { if (mTitle == t) return; mTitle = t; emit titleChanged(); }
    void setCreatedAt(const QDateTime& dt)      { mCreatedAt = dt; emit createdAtChanged(); }
    void setTranscription(const QString& t)     { if (mTranscription  == t) return; mTranscription  = t; emit transcriptionChanged(); }
    void setConfig(const SessionConfig& cfg);

    // Heavy data lifecycle — called by SessionService on activate/deactivate
    void loadConversation(const QVector<Message>& messages);
    void unloadConversation();
    void loadTranscription(const QString& text);
    void unloadTranscription();

signals:
    void titleChanged();
    void createdAtChanged();
    void transcriptionChanged();
    void conversationLoaded();

private:
    int              mDbId        = -1;
    QString          mTitle;
    QDateTime        mCreatedAt   = QDateTime::currentDateTime();
    SessionConfig    mConfig;

    // Heavy — null until active
    Conversation* mConversation  = nullptr;
    QString       mTranscription;
};
