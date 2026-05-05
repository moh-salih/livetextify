#include "liveTextify/modules/services/ragservice.h"
#include "liveTextify/modules/services/databaseservice.h"
#include "liveTextify/modules/database/repositories/chunkrepository.h"
#include "liveTextify/modules/session/session.h"
#include "liveTextify/core/Logger.h"

#include <QtLlama/Embedder.h>
#include <QtLlama/EmbeddingWorker.h>
#include <QtRag/QtRag.h>

// ── RAG adapters ──────────────────────────────────────────────────────────────

class AppRagEmbedder : public QtRag::IRagEmbedder {
public:
    explicit AppRagEmbedder(QtLlama::Embedder* emb, QObject* parent = nullptr)
        : QtRag::IRagEmbedder(parent), m_emb(emb)
    {
        connect(m_emb, &QtLlama::Embedder::embeddingReady,this, &QtRag::IRagEmbedder::embeddingReady);

        connect(m_emb, &QtLlama::Embedder::errorOccurred,this, [this](QtLlama::Error) {
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

    bool saveChunk(int sessionId, const QString& text, int chunkIndex, const std::vector<float>& embedding) override {
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

// ── RagService ────────────────────────────────────────────────────────────────

RagService::RagService(DatabaseService* database, QObject* parent): QObject(parent), m_database(database), m_embedder(new QtLlama::Embedder(this))
{
    m_embedder->initialize(new QtLlama::EmbeddingWorker());

    m_ragEmbedderAdapter = std::make_unique<AppRagEmbedder>(m_embedder);
    m_ragStorageAdapter  = std::make_unique<AppRagStorage>(m_database);

    // Build the engine with default config — rebuilt on session change
    m_ragEngine = std::make_unique<QtRag::RagEngine>(m_ragEmbedderAdapter.get(), m_ragStorageAdapter.get(), QtRag::RagConfig(), this);

    connect(m_ragEngine.get(), &QtRag::RagEngine::contextReady, this, &RagService::contextReady);

    connect(m_ragEngine.get(), &QtRag::RagEngine::errorOccurred, this, &RagService::errorOccurred);

    connect(m_embedder, &QtLlama::Embedder::statusChanged, this, &RagService::embedderStatusChanged);

    connect(m_embedder, &QtLlama::Embedder::reloadRequired, this, &RagService::reloadRequired);

    connect(m_embedder, &QtLlama::Embedder::errorOccurred, this, &RagService::embedderErrorOccurred);
}

RagService::~RagService() = default;

bool RagService::isReady() const {
    return m_embedder->status() == QtLlama::Status::Ready;
}


QtLlama::Status RagService::embedderStatus() const {
    return m_embedder ? m_embedder->status(): QtLlama::Status::Idle;
}

// ── Session lifecycle ─────────────────────────────────────────────────────────

void RagService::onActiveSessionChanged(Session* activeSession) {
    m_activeSession       = activeSession;
    m_lastProcessedLength = 0;

    if (!activeSession) { unloadModels(); return; }

    m_ragEngine->resetSession(activeSession->sessionID());
    loadModels(activeSession);
    rebuildIndex();

    Logger::info("[RagService] Active session changed, embedding model: " + activeSession->config().emb.modelPath);
}

void RagService::onConfigChanged(const SessionConfig& config) {
    m_embedder->setConfig(config.emb);

    // Rebuild the RAG engine with the updated RAG config
    m_ragEngine = std::make_unique<QtRag::RagEngine>(m_ragEmbedderAdapter.get(), m_ragStorageAdapter.get(), config.rag, this);

    connect(m_ragEngine.get(), &QtRag::RagEngine::contextReady, this, &RagService::contextReady);

    connect(m_ragEngine.get(), &QtRag::RagEngine::errorOccurred, this, &RagService::errorOccurred);

}

// ── Transcription indexing ────────────────────────────────────────────────────

void RagService::onTranscriptionUpdated(const QString& fullText, int sessionId) {
    if (!m_activeSession) return;
    if (m_activeSession->sessionID() != sessionId) return;
    if (!m_ragEngine) return;

    if (fullText.length() > m_lastProcessedLength + 50) {
        m_ragEngine->indexText(fullText.mid(m_lastProcessedLength), sessionId);
        m_lastProcessedLength = fullText.length();
    }
}

// ── Context request ───────────────────────────────────────────────────────────

void RagService::requestContext(const QString& text, int sessionId) {
    if (!m_activeSession || m_activeSession->sessionID() != sessionId) return;
    if (!m_ragEngine) return;
    m_ragEngine->requestContext(text, sessionId);
}

// ── Reload ────────────────────────────────────────────────────────────────────

void RagService::reloadModels() {
    if (!m_activeSession) return;
    loadModels(m_activeSession);
}

// ── Internal ──────────────────────────────────────────────────────────────────

void RagService::loadModels(Session* activeSession) {
    m_embedder->setConfig(activeSession->config().emb);
    m_embedder->loadModel();
}

void RagService::unloadModels() {
    m_embedder->unloadModel();
}

void RagService::rebuildIndex() {
    if (!m_activeSession || !m_ragEngine) return;
    m_ragEngine->rebuildIndex(m_activeSession->sessionID());
}
