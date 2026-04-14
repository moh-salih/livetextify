#include "conversationservice.h"
#include "liveTextify/modules/session/session.h"
#include "liveTextify/modules/engines/llm/llmsession.h"
#include "liveTextify/modules/engines/stt/sttsession.h"
#include "liveTextify/modules/settings/settingsmanager.h"
#include "liveTextify/modules/services/databaseservice.h"
#include "liveTextify/core/Logger.h"

ConversationService::ConversationService(SettingsManager * settings, DatabaseService * database, AiService* aiService, SessionService* sessionService, QObject* parent)
    : QObject(parent), mDatabaseService(database), m_ai(aiService), m_sessions(sessionService)
{
    connect(m_sessions, &SessionService::activeSessionChanged, this, &ConversationService::onActiveSessionChanged);
    connect(m_ai->llmSession(), &LlmSession::textGenerated, this, &ConversationService::onLLMTextGenerated);
    connect(m_ai->llmSession(), &LlmSession::isGeneratingChanged, this, &ConversationService::onLLMStateChanged);
    connect(m_ai->sttSession(), &SttSession::partialResult, this, &ConversationService::onSTTPartialResult);
}

void ConversationService::onActiveSessionChanged() {
    m_currentGeneratingSessionId = -1;
    m_waitingForContext = false;
}

void ConversationService::handleUserMessage(const QString& text, int sessionId) {
    Session* active = m_sessions->activeSession();
    if (!active || active->sessionID() != sessionId) return;

    Message userMsg("user", text, true);
    mDatabaseService->saveMessage(sessionId, userMsg, active->conversation()->messages()->count());

    if (active->enableRag()) {
        m_waitingForContext = true;
        m_pendingUserMessage = text;
        m_pendingSessionId = sessionId;
        emit requestContext(text, sessionId);
    } else {
        generateLlmPrompt(text, sessionId);
    }
}

void ConversationService::onContextReady(const QString& context, int sessionId) {
    if (!m_waitingForContext || sessionId != m_pendingSessionId) return;
    m_waitingForContext = false;

    QString enrichedMessage = m_pendingUserMessage;
    if (!context.isEmpty()) {
        enrichedMessage = QString("Relevant fragments from the live speech transcription:\n%1\n\nUser Question: %2")
        .arg(context).arg(m_pendingUserMessage);
    }
    generateLlmPrompt(enrichedMessage, sessionId);
}

void ConversationService::generateLlmPrompt(const QString& currentInput, int sessionId) {
    Session* active = m_sessions->activeSession();
    if (!active) return;

    // 1. Construct sliding window history (Gemma style)
    QString historyStr;
    auto* msgModel = active->conversation()->messages();

    // Grab the last 4 messages for context (2 exchanges)
    int start = std::max(0, msgModel->count() - 5);
    for (int i = start; i < msgModel->count() - 1; ++i) {
        auto m = msgModel->get(i);
        if (m.role == "user") historyStr += "User: " + m.content + "\n";
        else historyStr += "Assistant: " + m.content + "\n";
    }

    // 2. Final prompt using strict start/end turn tags
    QString prompt = QString("<start_of_turn>user\n%1\n\n%2\n%3<end_of_turn>\n<start_of_turn>model\n")
                         .arg(active->systemPrompt())
                         .arg(historyStr)
                         .arg(currentInput);

    Logger::info(QString("ConversationService: Prompting LLM for session %1").arg(sessionId));

    m_currentGeneratingSessionId = sessionId;
    m_ai->llmSession()->generate(prompt, sessionId);
}


void ConversationService::onSTTPartialResult(const QString& text) {
    Session* active = m_sessions->activeSession();
    if (!active) return;

    QString current = active->transcription();
    QString updated = current.isEmpty() ? text : current + " " + text;
    active->setTranscription(updated);

    mDatabaseService->saveTranscription(active->sessionID(), updated);
    emit transcriptionUpdated(updated, active->sessionID());
}

void ConversationService::onLLMTextGenerated(const QString& text, int sessionId) {
    Session* active = m_sessions->activeSession();
    if (!active || active->sessionID() != sessionId) return;

    auto* msgModel = active->conversation()->messages();

    if (msgModel->count() > 0) {
        auto last = msgModel->get(msgModel->count() - 1);
        if (last.role == "assistant" && !last.isComplete) {
            Message updated("assistant", last.content + text, false);
            active->conversation()->updateMessage(msgModel->count() - 1, updated);
        } else {
            active->conversation()->addMessage("assistant", text, false);
        }
    } else {
        active->conversation()->addMessage("assistant", text, false);
    }

    emit assistantMessageChunk(text, sessionId);
}

void ConversationService::onLLMStateChanged(bool generating) {
    if (!generating && m_currentGeneratingSessionId != -1) {
        Session* active = m_sessions->activeSession();
        if (active) {
            auto* msgModel = active->conversation()->messages();
            int lastIdx = msgModel->count() - 1;
            auto last = msgModel->get(lastIdx);

            if (last.role == "assistant" && !last.isComplete) {
                Message complete("assistant", last.content, true);
                active->conversation()->updateMessage(lastIdx, complete);
                mDatabaseService->saveMessage(active->sessionID(), complete, lastIdx);
                emit assistantMessageComplete(active->sessionID());
            }
        }
        m_currentGeneratingSessionId = -1;
    }
}
