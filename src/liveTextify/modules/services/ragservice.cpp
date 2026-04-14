#include "ragservice.h"
#include "liveTextify/modules/engines/emb/embsession.h"
#include "liveTextify/modules/engines/stt/sttsession.h"
#include "liveTextify/modules/rag/ragtextchunker.h"
#include "liveTextify/modules/services/aiservice.h"
#include "liveTextify/modules/services/sessionservice.h"
#include "liveTextify/modules/services/databaseservice.h"
#include "liveTextify/modules/settings/settingsmanager.h"

#include "liveTextify/core/Logger.h"

#include <algorithm>
#include <QReadLocker>
#include <QWriteLocker>

RagService::RagService(SettingsManager * settings, DatabaseService * database, AiService* aiService, SessionService* sessionService, QObject* parent)
    : QObject(parent),
    mRAGConfigManager(settings->ragConfigManager()),
    mDatabaseService(database),
    mAI(aiService),
    mSessions(sessionService){

    connect(mAI->embSession(), &EmbSession::embeddingReady, this, &RagService::onEmbeddingReady);

    connect(mAI->embSession(), &EmbSession::errorOccurred, this, [this](const QString& err){
        Logger::error(QString("RagService: Embedding engine error: %1").arg(err));
        handleTaskFailure();
    });

    connect(mSessions, &SessionService::activeSessionChanged, this, &RagService::onActiveSessionChanged);

    connect(mAI->sttSession(), &SttSession::transcriptionChanged, this, [this](const QString& fullText){
        Session* active = mSessions->activeSession();
        if (!active || !active->enableRag()) return;

        if (fullText.length() > mLastProcessedLength + 50) {
            QString newPart = fullText.mid(mLastProcessedLength);
            this->indexText(newPart, active->sessionID());
            mLastProcessedLength = fullText.length();
        }
    });
}

void RagService::onActiveSessionChanged() {
    Session* active = mSessions->activeSession();

    {
        QMutexLocker locker(&mQueueMutex); // Lock Queue
        mLastProcessedLength = 0;
        while(!mTaskQueue.empty()) mTaskQueue.pop();
        mIsProcessing = false;
    } // Unlock Queue

    if (active && active->enableRag()) {
        rebuildVectorIndex(active->sessionID());
    }
}

void RagService::handleTaskFailure() {
    EmbeddingTask task;
    bool hasTask = false;

    {
        QMutexLocker locker(&mQueueMutex); // Lock Queue
        if (!mTaskQueue.empty()) {
            task = mTaskQueue.front();
            mTaskQueue.pop();
            hasTask = true;
        }
        mIsProcessing = false;
    } // Unlock Queue

    // Emit signals outside the lock to prevent deadlocks
    if (hasTask && task.chunkIndex < 0) {
        emit contextReady("", task.sessionId);
    }
    processNextInQueue();
}

void RagService::onEmbeddingReady(const std::vector<float>& embedding, const QString& text, int chunkIndex) {
    EmbeddingTask task;

    {
        QMutexLocker locker(&mQueueMutex); // Lock Queue
        if (mTaskQueue.empty()) {
            mIsProcessing = false;
            return;
        }
        task = mTaskQueue.front();
        mTaskQueue.pop();
        mIsProcessing = false;
    } // Unlock Queue BEFORE doing heavy database/index work

    if (chunkIndex >= 0) {
        // Path: Storing a new chunk from speech
        bool saved = mDatabaseService->saveChunk(task.sessionId, text, chunkIndex, embedding);

        const auto& ragConfig = mRAGConfigManager->config();

        if (saved && embedding.size() == static_cast<size_t>(ragConfig.dim)) {
            QWriteLocker writeLocker(&mIndexLock); // Lock HNSW for WRITING
            if (mHnswIndex) {
                ChunkData newData{0, task.sessionId, text, chunkIndex, embedding};
                mLoadedChunks.append(newData);
                mHnswIndex->addPoint(embedding.data(), mLoadedChunks.size() - 1);
            }
        }
    } else {
        // Path: Searching for context for a user question
        QString context = performHnswSearch(embedding, task.sessionId);
        emit contextReady(context, task.sessionId);
    }

    processNextInQueue();
}

void RagService::processNextInQueue() {
    QMutexLocker locker(&mQueueMutex); // Lock Queue
    if (mIsProcessing || mTaskQueue.empty()) return;

    mIsProcessing = true;
    EmbeddingTask task = mTaskQueue.front();
    locker.unlock(); // Unlock immediately before calling external methods

    mAI->embSession()->generateEmbedding(task.text, task.chunkIndex);
}

void RagService::indexText(const QString& text, int sessionId) {
    QStringList chunks = RagTextChunker::splitIntoHighQualityChunks(text, mRAGConfigManager->config());
    if (chunks.isEmpty()) return;

    {
        QMutexLocker locker(&mQueueMutex); // Lock Queue
        foreach(const QString& chunk, chunks) {
            EmbeddingTask task;
            task.text = chunk;
            task.sessionId = sessionId;
            task.chunkIndex = getNextIndex(sessionId);
            mTaskQueue.push(task);
        }
    } // Unlock Queue

    processNextInQueue();
}

void RagService::requestContext(const QString& query, int sessionId) {
    {
        QMutexLocker locker(&mQueueMutex); // Lock Queue
        EmbeddingTask task;
        task.text = query;
        task.sessionId = sessionId;
        task.chunkIndex = -1; // Flag for search
        mTaskQueue.push(task);
    } // Unlock Queue

    processNextInQueue();
}

QString RagService::performHnswSearch(const std::vector<float>& queryVec, int sessionId) {
    QReadLocker readLocker(&mIndexLock); // Lock HNSW for READING
    const auto& ragConfig = mRAGConfigManager->config();
    if (!mHnswIndex || queryVec.size() != static_cast<size_t>(ragConfig.dim) || mLoadedChunks.isEmpty()) return "";

    try {
        auto result = mHnswIndex->searchKnn(queryVec.data(), ragConfig.topK);
        QString context;

        struct Result { float dist; size_t id; };
        std::vector<Result> sorted;
        while(!result.empty()) {
            sorted.push_back({result.top().first, result.top().second});
            result.pop();
        }
        std::reverse(sorted.begin(), sorted.end());

        for (const auto& res : sorted) {
            if (res.id < (size_t)mLoadedChunks.size()) {
                context += "- " + mLoadedChunks[res.id].text + "\n";
            }
        }
        return context.trimmed();
    } catch (...) {
        return "";
    }
}

void RagService::rebuildVectorIndex(int sessionId) {
    const auto& ragConfig = mRAGConfigManager->config();

    // 1. Do the heavy database read completely OUTSIDE the lock
    QVector<ChunkData> chunks = mDatabaseService->loadChunks(sessionId);

    auto space = new hnswlib::L2Space(static_cast<size_t>(ragConfig.dim));
    size_t maxElements = std::max((size_t)500, (size_t)chunks.size() * 2);
    auto index = new hnswlib::HierarchicalNSW<float>(space, maxElements, 16, 200);

    for (int i = 0; i < chunks.size(); ++i) {
        if (chunks[i].embedding.size() == static_cast<size_t>(ragConfig.dim)) {
            index->addPoint(chunks[i].embedding.data(), i);
        }
    }

    // 2. Only lock for the microsecond it takes to swap the pointers
    QWriteLocker writeLocker(&mIndexLock);

    if (mHnswIndex) delete mHnswIndex;
    if (mSpace) delete mSpace;

    mLoadedChunks = chunks;
    mSpace = space;
    mHnswIndex = index;
}

int RagService::getNextIndex(int sessionId) {
    // Expected to be called from inside mQueueMutex lock
    if (!mCounters.contains(sessionId)) {
        mCounters[sessionId] = mDatabaseService->getChunkCount(sessionId);
    }
    return mCounters[sessionId]++;
}

RagService::~RagService() {
    QWriteLocker writeLocker(&mIndexLock);
    if (mHnswIndex) delete mHnswIndex;
    if (mSpace) delete mSpace;
}
