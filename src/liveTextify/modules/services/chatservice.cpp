#include "chatservice.h"
#include "liveTextify/modules/services/databaseservice.h"
#include "liveTextify/modules/database/repositories/chunkrepository.h"
#include "liveTextify/core/Logger.h"

#include <QtLlama/Session.h>
#include <QtLlama/Engine.h>
#include <QtLlama/Embedder.h>
#include <QtLlama/EmbeddingWorker.h>
#include <QtRag/QtRag.h>

// ── RAG adapters ─────────────────────────────────────────────────────────────

class AppRagEmbedder : public QtRag::IRagEmbedder {
public:
    explicit AppRagEmbedder(QtLlama::Embedder* emb, QObject* parent = nullptr)
        : QtRag::IRagEmbedder(parent), m_emb(emb)
    {

        connect(m_emb, &QtLlama::Embedder::embeddingReady, this,  &QtRag::IRagEmbedder::embeddingReady);

        connect(m_emb, &QtLlama::Embedder::errorOccurred, this, [this](QtLlama::Error /*e*/) {
                    emit QtRag::IRagEmbedder::errorOccurred(QtRag::Error::EmbeddingFailed);
                });

    }

    void generateEmbedding(const QString& text, int chunkIndex) override {
        m_emb->generateEmbedding(text, chunkIndex);
    }
private:
    QtLlama::Embedder* m_emb;
};

class AppRagStorage : public QtRag::IRagStorage {
public:
    explicit AppRagStorage(DatabaseService* db) : m_db(db) {}
    bool saveChunk(int sessionId, const QString& text, int chunkIndex,
                   const std::vector<float>& embedding) override {
        return m_db->saveChunk(sessionId, text, chunkIndex, embedding);
    }
    QVector<QtRag::RagChunk> loadChunks(int sessionId) override {
        QVector<QtRag::RagChunk> out;
        for (const auto& c : m_db->loadChunks(sessionId))
            out.push_back({c.id, c.sessionId, c.chunkIndex, c.text, c.embedding});
        return out;
    }
    int  getChunkCount(int sessionId) override { return m_db->getChunkCount(sessionId); }
    void clearSession(int sessionId)  override { m_db->deleteChunks(sessionId); }
private:
    DatabaseService* m_db;
};

// ── ChatService ───────────────────────────────────────────────────────────────

ChatService::ChatService(DatabaseService* database, QObject* parent)
    : QObject(parent)
    , m_database(database)
    , m_llama(new QtLlama::Session(this))
    , m_embedder(new QtLlama::Embedder(this))
{
    m_llama->initialize(new QtLlama::Engine());
    m_embedder->initialize(new QtLlama::EmbeddingWorker());

    m_ragEmbedderAdapter = std::make_unique<AppRagEmbedder>(m_embedder);
    m_ragStorageAdapter  = std::make_unique<AppRagStorage>(m_database);

    updateRagConfig();

    // ── Status ────────────────────────────────────────────────────────────────
    connect(m_llama, &QtLlama::Session::statusChanged,
            this, &ChatService::llamaStatusChanged);

    connect(m_embedder, &QtLlama::Embedder::statusChanged,
            this, &ChatService::embedderStatusChanged);

    // ── Generation ────────────────────────────────────────────────────────────
    connect(m_llama, &QtLlama::Session::textGenerated,
            this, &ChatService::onLLMTextGenerated);

    connect(m_llama, &QtLlama::Session::isGeneratingChanged,
            this, [this](bool generating) {
                emit isLlamaGeneratingChanged(generating);
                onLLMStateChanged(generating);
            });

    // ── Errors ────────────────────────────────────────────────────────────────
    connect(m_llama, &QtLlama::Session::errorOccurred,
            this, &ChatService::llamaErrorOccurred);

    connect(m_embedder, &QtLlama::Embedder::errorOccurred,
            this, &ChatService::embedderErrorOccurred);
}

ChatService::~ChatService() = default;

QtLlama::Status ChatService::llamaStatus()    const { return m_llama->status(); }
QtLlama::Status ChatService::embedderStatus() const { return m_embedder->status(); }


void ChatService::stopGeneration() {
    m_llama->stop();
}

void ChatService::updateRagConfig() {
    m_ragEngine = std::make_unique<QtRag::RagEngine>(
        m_ragEmbedderAdapter.get(),
        m_ragStorageAdapter.get(),
        QtRag::RagConfig(),
        this);

    connect(m_ragEngine.get(), &QtRag::RagEngine::contextReady,
            this, &ChatService::onContextReady);

    connect(m_ragEngine.get(), &QtRag::RagEngine::errorOccurred,
            this, &ChatService::ragErrorOccurred);
}

void ChatService::onActiveSessionChanged(Session* activeSession) {
    m_activeSession = activeSession;
    m_currentGeneratingSessionId = -1;
    m_waitingForContext = false;
    m_lastProcessedLength = 0;

    if (!activeSession) { unloadModels(); return; }

    if (activeSession->config().enableRag && m_ragEngine) {
        m_ragEngine->resetSession(activeSession->sessionID());
        m_ragEngine->rebuildIndex(activeSession->sessionID());
    }

    loadModels(activeSession);
}

void ChatService::onActiveSessionConfigChanged(const SessionConfig& config) {
    m_llama->setConfig(config.llm);
    m_embedder->setConfig(config.emb);
    m_llama->reloadModel();
    m_embedder->reloadModel();
}

void ChatService::loadModels(Session* activeSession) {
    const auto& config = activeSession->config();
    m_llama->setConfig(config.llm);     // triggers loadModel() inside engine
    m_embedder->setConfig(config.emb);  // same
    m_llama->reloadModel();
    m_embedder->reloadModel();
}

void ChatService::unloadModels() {
    m_llama->unloadModel();
    m_embedder->unloadModel();
}

void ChatService::onUserMessageReady(const QString& text, int sessionId) {
    if (!m_activeSession || m_activeSession->sessionID() != sessionId) return;

    m_activeSession->conversation()->addMessage("user", text, true);
    const int messageIndex = m_activeSession->conversation()->messages()->count() - 1;
    m_database->saveMessage(sessionId, Message("user", text, true), messageIndex);

    if (m_activeSession->config().enableRag && m_ragEngine) {
        m_waitingForContext  = true;
        m_pendingUserMessage = text;
        m_pendingSessionId   = sessionId;
        m_ragEngine->requestContext(text, sessionId);
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

void ChatService::onTranscriptionUpdated(const QString& fullText, int sessionId) {
    if (!m_activeSession) return;
    if (!m_activeSession->config().enableRag) return;
    if (m_activeSession->sessionID() != sessionId) return;
    if (!m_ragEngine) return;

    if (fullText.length() > m_lastProcessedLength + 50) {
        m_ragEngine->indexText(fullText.mid(m_lastProcessedLength), sessionId);
        m_lastProcessedLength = fullText.length();
    }
}

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
