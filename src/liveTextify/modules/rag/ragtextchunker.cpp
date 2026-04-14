#include "liveTextify/modules/rag/ragtextchunker.h"
#include <QRegularExpression>
#include <cmath>
#include <algorithm>

QStringList RagTextChunker::splitIntoHighQualityChunks(const QString& text, const RAGConfig& config) {
    if (text.trimmed().isEmpty()) return {};

    // Split by common sentence delimiters
    static const QRegularExpression sentenceEnd("[.!?]\\s+");
    QStringList rawSentences = text.split(sentenceEnd, Qt::SkipEmptyParts);

    QStringList result;
    QString buffer;

    for (const QString& rawS : rawSentences) {
        QString s = rawS.trimmed();
        if (s.isEmpty()) continue;
        if (!s.endsWith('.') && !s.endsWith('!') && !s.endsWith('?')) s += ".";

        // Greedy Merging: Add sentences to buffer until maxChars is reached
        if (buffer.isEmpty()) {
            buffer = s;
        } else if (buffer.length() + s.length() < config.maxChars) {
            buffer += " " + s;
        } else {
            // Buffer is full. Validate quality before adding to index list
            if (checkQuality(buffer, config)) result.append(buffer);
            buffer = s;
        }
    }

    // Process remaining tail
    if (!buffer.isEmpty() && checkQuality(buffer, config)) {
        result.append(buffer);
    }

    return result;
}

QString RagTextChunker::rankRelevantContext(const std::vector<float>& queryVec, const QVector<ChunkData>& dbChunks, const RAGConfig& config) {
    if (dbChunks.isEmpty() || queryVec.empty()) return "";

    struct Candidate { float score; QString text; };
    std::vector<Candidate> candidates;

    for (const auto& chunk : dbChunks) {
        if (chunk.embedding.empty()) continue;

        float sim = calculateCosineSimilarity(queryVec, chunk.embedding);

        // Length Normalization: Give a slight boost to chunks that provide more "meat"
        // This prevents the system from returning too many one-liners.
        float lengthRatio = std::min(1.0f, static_cast<float>(chunk.text.length()) / config.maxChars);
        float bonus = lengthRatio * config.lengthBoost;
        float finalScore = sim + bonus;

        if (finalScore >= config.minScore) {
            candidates.push_back({finalScore, chunk.text});
        }
    }

    // Sort Descending (highest score first)
    std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
        return a.score > b.score;
    });

    // Format the top K matches for the LLM prompt
    QString contextString;
    int limit = std::min(config.topK, static_cast<int>(candidates.size()));
    for (int i = 0; i < limit; ++i) {
        contextString += QString("- %1\n").arg(candidates[i].text);
    }

    return contextString.trimmed();
}

bool RagTextChunker::checkQuality(const QString& text, const RAGConfig& config) {
    if (text.length() < config.minChars) return false;

    // Detect high-frequency speech filler words
    static const QStringList fillers = {"i mean", "you know", "actually", "basically", "like", "um", "uh"};
    int fillerCount = 0;
    QString lower = text.toLower();

    for (const auto& f : fillers) {
        if (lower.contains(f)) fillerCount++;
    }

    // Simple word count estimate
    int wordCount = text.count(' ') + 1;
    return (static_cast<float>(fillerCount) / wordCount) <= config.fillerLimit;
}

float RagTextChunker::calculateCosineSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size() || a.empty()) return 0.0f;

    float dot = 0.0f, magA = 0.0f, magB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        magA += a[i] * a[i];
        magB += b[i] * b[i];
    }

    float den = std::sqrt(magA) * std::sqrt(magB);
    return (den < 1e-9f) ? 0.0f : (dot / den);
}
