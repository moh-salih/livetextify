#pragma once
#include <QString>
#include <QStringList>
#include <QVector>
#include <vector>
#include "liveTextify/modules/database/repositories/chunkrepository.h"
#include "liveTextify/modules/engines/common.h"

class RagTextChunker {
public:
    static QStringList splitIntoHighQualityChunks(const QString& text, const RAGConfig& config = RAGConfig());
    static QString rankRelevantContext(const std::vector<float>& queryVec, const QVector<ChunkData>& dbChunks, const RAGConfig& config = RAGConfig());
private:
    // Internal helper methods for math and filtering
    static float calculateCosineSimilarity(const std::vector<float>& a, const std::vector<float>& b);
    static bool checkQuality(const QString& text, const RAGConfig& config = RAGConfig());
};
