#include "liveTextify/modules/database/repositories/chunkrepository.h"
#include "liveTextify/modules/database/databasemanager.h"

#include "liveTextify/core/Logger.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>


ChunkRepository::ChunkRepository(DatabaseManager* db, QObject *parent)
    : QObject(parent), m_db(db) {}

bool ChunkRepository::saveChunk(int sessionId, const QString& text, int chunkIndex, const std::vector<float>& embedding) {
    Logger::info("===== DATABASE SAVE =====");
    Logger::info(QString("Saving chunk - Session: %1, Index: %2, Text length: %3").arg(sessionId).arg(chunkIndex).arg(text.length()));
    Logger::debug(QString("Text: %1").arg(text));


    QSqlQuery query(m_db->database());
    query.prepare(
        "INSERT INTO chunks (session_id, text, chunk_index, vector) "
        "VALUES (?, ?, ?, ?)"
        );

    query.addBindValue(sessionId);
    query.addBindValue(text);
    query.addBindValue(chunkIndex);

    QByteArray blob(reinterpret_cast<const char*>(embedding.data()), embedding.size() * sizeof(float));
    query.addBindValue(blob);

    bool success = query.exec();


    if (success) {
        Logger::info(QString("Chunk saved to database (ID: %1)").arg(query.lastInsertId().toLongLong()));
    } else {
        Logger::error(QString("Database error: %1").arg(query.lastError().text()));
    }

    return success;
}

QVector<ChunkData> ChunkRepository::loadChunks(int sessionId) {
    Logger::info("===== DATABASE LOAD =====");
    Logger::info(QString("Loading chunks for session %1").arg(sessionId));


    QVector<ChunkData> chunks;
    QSqlQuery query(m_db->database());
    query.prepare("SELECT id, text, chunk_index, vector FROM chunks WHERE session_id = ? ORDER BY chunk_index ASC");
    query.addBindValue(sessionId);

    if (query.exec()) {
        while (query.next()) {
            ChunkData data;
            data.id = query.value(0).toInt();
            data.sessionId = sessionId;
            data.text = query.value(1).toString();
            data.chunkIndex = query.value(2).toInt();

            QByteArray blob = query.value(3).toByteArray();
            if (!blob.isEmpty()) {
                const float* ptr = reinterpret_cast<const float*>(blob.constData());
                size_t numFloats = blob.size() / sizeof(float);
                data.embedding.assign(ptr, ptr + numFloats);
            }
            chunks.append(data);


            Logger::debug(QString("Loaded chunk %1 - Index: %2, Text: '%3'").arg(data.id).arg(data.chunkIndex).arg(data.text.left(30)));
        }
        Logger::info(QString("Loaded %1 chunks from database").arg(chunks.size()));
    } else {
        Logger::error(QString("Failed to load chunks: %1").arg(query.lastError().text()));
    }

    return chunks;
}

bool ChunkRepository::deleteChunks(int sessionId) {
    QSqlQuery query(m_db->database());
    query.prepare("DELETE FROM chunks WHERE session_id = ?");
    query.addBindValue(sessionId);
    return query.exec();
}

int ChunkRepository::getChunkCount(int sessionId) {
    QSqlQuery query(m_db->database());
    query.prepare("SELECT COUNT(*) FROM chunks WHERE session_id = ?");
    query.addBindValue(sessionId);
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}
