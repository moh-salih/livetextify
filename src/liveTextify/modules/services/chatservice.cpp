#include "chatservice.h"
#include "liveTextify/modules/services/databaseservice.h"
#include "liveTextify/core/Logger.h"

#include <QtLlama/Session.h>
#include <QtLlama/Engine.h>

ChatService::ChatService(DatabaseService* database, QObject* parent)
    : QObject(parent)
    , m_database(database)
    , m_llama(new QtLlama::Session(this))
{
    m_llama->initialize(new QtLlama::Engine());

    connect(m_llama, &QtLlama::Session::statusChanged,
            this, &ChatService::llamaStatusChanged);

    connect(m_llama, &QtLlama::Session::reloadRequired,
            this, &ChatService::reloadRequired);

    connect(m_llama, &QtLlama::Session::contextLengthResolved,
            this, &ChatService::contextLengthResolved);

    connect(m_llama, &QtLlama::Session::textGenerated,
            this, &ChatService::onLLMTextGenerated);

    connect(m_llama, &QtLlama::Session::isGeneratingChanged,
            this, [this](bool generating) {
                emit isLlamaGeneratingChanged(generating);
                onLLMStateChanged(generating);
            });

    connect(m_llama, &QtLlama::Session::errorOccurred,
            this, &ChatService::llamaErrorOccurred);
}

ChatService::~ChatService() = default;

QtLlama::Status ChatService::llamaStatus() const { return m_llama->status(); }
bool            ChatService::isGenerating() const { return m_llama ? m_llama->isGenerating() : false; }

void ChatService::reloadModels() {
    if (!m_activeSession) return;
    loadModels(m_activeSession);
}

void ChatService::stopGeneration() {
    m_llama->stop();
}

// ── Session lifecycle ─────────────────────────────────────────────────────────

void ChatService::onActiveSessionChanged(Session* activeSession) {
    m_activeSession              = activeSession;
    m_currentGeneratingSessionId = -1;
    m_waitingForContext          = false;

    if (!activeSession) { unloadModels(); return; }

    loadModels(activeSession);
}

void ChatService::loadModels(Session* activeSession) {
    m_llama->setConfig(activeSession->config().llm);
    m_llama->loadModel();
}

void ChatService::onConfigChanged(const SessionConfig& config) {
    m_llama->setConfig(config.llm);
}

void ChatService::unloadModels() {
    m_llama->unloadModel();
}

// ── Message handling ──────────────────────────────────────────────────────────

void ChatService::onUserMessageReady(const QString& text, int sessionId) {
    if (!m_activeSession || m_activeSession->sessionID() != sessionId) return;

    m_activeSession->conversation()->addMessage("user", text, true);
    const int messageIndex = m_activeSession->conversation()->messages()->count() - 1;
    m_database->saveMessage(sessionId, Message("user", text, true), messageIndex);

    if (m_activeSession->config().enableRag) {
        // Signal approach — decoupled from RagService, SessionManager wires the connection
        m_waitingForContext  = true;
        m_pendingUserMessage = text;
        m_pendingSessionId   = sessionId;
        emit contextRequested(text, sessionId);
    } else {
        generateLlmPrompt(text, sessionId);
    }
}

void ChatService::onContextReady(const QString& context, int sessionId) {
    if (!m_waitingForContext || sessionId != m_pendingSessionId) return;
    m_waitingForContext = false;

    QString enriched = m_pendingUserMessage;
    if (!context.isEmpty()) {
        enriched = QString("Relevant fragments from the live speech transcription:\n%1\n\nUser Question: %2")
        .arg(context).arg(m_pendingUserMessage);
    }
    generateLlmPrompt(enriched, sessionId);
}

void ChatService::generateLlmPrompt(const QString& currentInput, int sessionId) {
    if (!m_activeSession) return;

    auto* msgModel = m_activeSession->conversation()->messages();
    QList<QtLlama::Message> messages;

    messages.append({QtLlama::Role::System, m_activeSession->config().systemPrompt});

    int start = std::max(0, msgModel->count() - 5);
    for (int i = start; i < msgModel->count() - 1; ++i) {
        auto m = msgModel->get(i);
        QtLlama::Role role = (m.role == "user") ? QtLlama::Role::User
                                                : QtLlama::Role::Assistant;
        messages.append({role, m.content});
    }
    messages.append({QtLlama::Role::User, currentInput});

    m_currentGeneratingSessionId = sessionId;
    m_llama->generate(messages, sessionId);
}

// ── LLM state ─────────────────────────────────────────────────────────────────

void ChatService::onLLMTextGenerated(const QString& text, int sessionId) {
    if (!m_activeSession || m_activeSession->sessionID() != sessionId) return;

    auto* msgModel = m_activeSession->conversation()->messages();
    if (msgModel->count() > 0) {
        auto last = msgModel->get(msgModel->count() - 1);
        if (last.role == "assistant" && !last.isComplete) {
            m_activeSession->conversation()->updateMessage(
                msgModel->count() - 1,
                Message("assistant", last.content + text, false));
        } else {
            m_activeSession->conversation()->addMessage("assistant", text, false);
        }
    } else {
        m_activeSession->conversation()->addMessage("assistant", text, false);
    }

    emit assistantMessageChunk(text, sessionId);
}

void ChatService::onLLMStateChanged(bool generating) {
    if (generating || m_currentGeneratingSessionId == -1) return;

    if (m_activeSession) {
        auto* msgModel = m_activeSession->conversation()->messages();
        int   lastIdx  = msgModel->count() - 1;
        auto  last     = msgModel->get(lastIdx);

        if (last.role == "assistant" && !last.isComplete) {
            Message complete("assistant", last.content, true);
            m_activeSession->conversation()->updateMessage(lastIdx, complete);
            m_database->saveMessage(m_activeSession->sessionID(), complete, lastIdx);
            emit assistantMessageComplete(m_activeSession->sessionID());
        }
    }
    m_currentGeneratingSessionId = -1;
}
