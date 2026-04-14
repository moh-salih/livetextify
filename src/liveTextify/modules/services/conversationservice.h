#pragma once

#include <QObject>
#include "liveTextify/modules/services//aiservice.h"
#include "liveTextify/modules/services/sessionservice.h"

class SettingsManager;
class DatabaseManage;

class ConversationService : public QObject {
    Q_OBJECT
public:
    explicit ConversationService(SettingsManager * settings, DatabaseService * database, AiService* aiService, SessionService* sessionService,QObject* parent = nullptr);

signals:
    // Orchestration signals (Caught by SessionManager to bridge with RagService)
    void requestContext(const QString& query, int sessionId);

    // UI Notification signals
    void assistantMessageChunk(const QString& text, int sessionId);
    void assistantMessageComplete(int sessionId);
    void transcriptionUpdated(const QString& text, int sessionId);

public slots:
    // Entry point for user messages (Chat Tab)
    void handleUserMessage(const QString& text, int sessionId);

    // Callback when RagService provides context
    void onContextReady(const QString& context, int sessionId);

private slots:
    // Reacting to session changes
    void onActiveSessionChanged();

    // Reacting to engine output
    void onSTTPartialResult(const QString& text);
    void onLLMTextGenerated(const QString& text, int sessionId);
    void onLLMStateChanged(bool generating);

private:
    void generateLlmPrompt(const QString& userText, int sessionId);

    // Dependencies
    AiService* m_ai;
    SessionService* m_sessions;
    DatabaseService * mDatabaseService;

    // Internal logic state
    bool m_waitingForContext = false;
    QString m_pendingUserMessage;
    int m_pendingSessionId = -1;
    int m_currentGeneratingSessionId = -1;
};
