#pragma once

#include <QObject>
#include <queue>
#include <QHash>
#include <vector>
#include <QMutex>
#include <QReadWriteLock>

// #include "liveTextify/modules/session/aiservice.h"
// #include "liveTextify/modules/session/sessionservice.h"
// #include "liveTextify/modules/engines/common.h"

#include <hnswlib/hnswlib.h>

class DatabaseService;
class AiService;
class SessionService;
struct ChunkData;
class RAGConfigManager;
class SettingsManager;
class RagService : public QObject {
    Q_OBJECT
public:
    explicit RagService(SettingsManager * settings, DatabaseService * database, AiService* aiService, SessionService* sessionService, QObject* parent = nullptr);
    ~RagService() override;

    void indexText(const QString& text, int sessionId);
    void requestContext(const QString& query, int sessionId);
    void clearSession(int sessionId);
    void stop();

signals:
    void contextReady(const QString& context, int sessionId);
    void errorOccurred(const QString& error);

private slots:
    void onEmbeddingReady(const std::vector<float>& embedding, const QString& text, int chunkIndex);
    void onActiveSessionChanged();

private:
    struct EmbeddingTask {
        QString text;
        int sessionId;
        int chunkIndex;
    };

    void processNextInQueue();
    void rebuildVectorIndex(int sessionId);
    void handleTaskFailure();
    QString performHnswSearch(const std::vector<float>& queryVec, int sessionId);
    int getNextIndex(int sessionId);

    // Dependencies
    AiService                       * mAI;
    SessionService                  * mSessions;
    DatabaseService                 * mDatabaseService;
    RAGConfigManager                * mRAGConfigManager;

    // HNSW memory structures (Protected by mIndexLock)
    hnswlib::HierarchicalNSW<float> * mHnswIndex    = nullptr;
    hnswlib::L2Space                * mSpace        = nullptr;
    QVector<ChunkData>                mLoadedChunks;
    QReadWriteLock                    mIndexLock;

    // State management (Protected by mQueueMutex)
    std::queue<EmbeddingTask>         mTaskQueue;
    QHash<int, int>                   mCounters;
    bool                              mIsProcessing = false;
    int                               mLastProcessedLength = 0;
    QMutex                            mQueueMutex;

};
