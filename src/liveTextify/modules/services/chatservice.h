#pragma once
#include <QObject>
#include <QtLlama/Types.h>
#include "liveTextify/modules/session/session.h"
#include "liveTextify/core/AppError.h"

namespace QtLlama { class Session; }

class DatabaseService;

class ChatService : public QObject {
    Q_OBJECT
public:
    explicit ChatService(DatabaseService* database, QObject* parent = nullptr);
    ~ChatService() override;

    QtLlama::Status llamaStatus()    const;
    bool            isGenerating()   const;

public slots:
    void onActiveSessionChanged(Session* activeSession);
    void onUserMessageReady(const QString& text, int sessionId);
    void onContextReady(const QString& context, int sessionId);
    void stopGeneration();
    void onConfigChanged(const SessionConfig& config);
    void reloadModels();

signals:
    void llamaStatusChanged(QtLlama::Status status);
    void isLlamaGeneratingChanged(bool generating);
    void assistantMessageChunk(const QString& text, int sessionId);
    void assistantMessageComplete(int sessionId);
    void contextLengthResolved(int length);
    void reloadRequired();

    // Signal approach — SessionManager wires this to RagService::requestContext
    // keeping ChatService fully decoupled from RAG
    void contextRequested(const QString& text, int sessionId);

    void llamaErrorOccurred(QtLlama::Error error);

private slots:
    void onLLMTextGenerated(const QString& text, int sessionId);
    void onLLMStateChanged(bool generating);

private:
    void generateLlmPrompt(const QString& userText, int sessionId);
    void loadModels(Session* activeSession);
    void unloadModels();

    Session*         m_activeSession = nullptr;
    DatabaseService* m_database;

    QtLlama::Session* m_llama;

    bool    m_waitingForContext          = false;
    QString m_pendingUserMessage;
    int     m_pendingSessionId           = -1;
    int     m_currentGeneratingSessionId = -1;
};
