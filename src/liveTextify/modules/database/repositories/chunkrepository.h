#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include <vector>

class DatabaseManager;

struct ChunkData {
    int id;
    int sessionId;
    QString text;
    int chunkIndex;
    std::vector<float> embedding;
};

class ChunkRepository : public QObject {
    Q_OBJECT
public:
    explicit ChunkRepository(DatabaseManager* db, QObject *parent = nullptr);

    bool saveChunk(int sessionId, const QString& text, int chunkIndex, const std::vector<float>& embedding);
    QVector<ChunkData> loadChunks(int sessionId);
    bool deleteChunks(int sessionId);
    int getChunkCount(int sessionId);

private:
    DatabaseManager* m_db;
};
