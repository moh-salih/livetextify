#include "liveTextify/modules/services/databaseservice.h"
#include "liveTextify/modules/database/databasemanager.h"
#include "liveTextify/modules/database/repositories/sessionrepository.h"
#include "liveTextify/modules/database/repositories/messagerepository.h"
#include "liveTextify/modules/database/repositories/transcriptionrepository.h"
#include "liveTextify/modules/database/repositories/chunkrepository.h"
#include "liveTextify/modules/session/SessionRecord.h"

DatabaseService::DatabaseService(QObject* parent): QObject(parent){
    auto* db = DatabaseManager::instance();
    mSessionRepo       = new SessionRepository(db, this);
    mMessageRepo       = new MessageRepository(db, this);
    mTranscriptionRepo = new TranscriptionRepository(db, this);
    mChunkRepo         = new ChunkRepository(db, this);
}

// Thin delegation — no logic, just forwarding
int DatabaseService::saveSession(const SessionRecord& rec) {
    return mSessionRepo->saveSession(rec);
}
bool DatabaseService::updateSession(const SessionRecord& rec) {
    return mSessionRepo->updateSession(rec);
}

bool DatabaseService::deleteSession(int sessionId){
    return mSessionRepo->deleteSession(sessionId);
}

bool DatabaseService::deleteAllSessions(){
    return mSessionRepo->deleteAll();
}

QList<SessionRecord> DatabaseService::loadAllSessions(){
    return mSessionRepo->loadAllSessions();
}

bool DatabaseService::saveMessage(int sessionId, const Message &msg, int index){
    return mMessageRepo->saveMessage(sessionId, msg, index);
}

bool DatabaseService::saveMessages(int sessionId, const QVector<Message> &messages){
    return mMessageRepo->saveMessages(sessionId, messages);
}

QVector<Message> DatabaseService::loadMessages(int sessionId){
    return mMessageRepo->loadMessages(sessionId);
}

bool DatabaseService::deleteMessages(int sessionId){
    return mMessageRepo->deleteMessages(sessionId);
}

bool DatabaseService::saveTranscription(int sessionId, const QString &text){
    return mTranscriptionRepo->saveTranscription(sessionId, text);
}

QString DatabaseService::loadTranscription(int sessionId){
    return mTranscriptionRepo->loadTranscription(sessionId);
}

bool DatabaseService::deleteTranscription(int sessionId){
    return mTranscriptionRepo->deleteTranscription(sessionId);
}

bool DatabaseService::saveChunk(int sessionId, const QString &text, int index, const std::vector<float> &embedding){
    return mChunkRepo->saveChunk(sessionId, text, index, embedding);
}

QVector<ChunkData> DatabaseService::loadChunks(int sessionId){
    return mChunkRepo->loadChunks(sessionId);
}

bool DatabaseService::deleteChunks(int sessionId){
    return mChunkRepo->deleteChunks(sessionId);
}

int DatabaseService::getChunkCount(int sessionId){
    return mChunkRepo->getChunkCount(sessionId);
}

