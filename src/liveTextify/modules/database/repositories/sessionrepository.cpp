#include "liveTextify/modules/database/repositories/sessionrepository.h"
#include "liveTextify/modules/session/session.h"
#include "liveTextify/modules/database/databasemanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

SessionRepository::SessionRepository(DatabaseManager* db, QObject *parent)
    : QObject(parent)
    , m_db(db)
{
}

int SessionRepository::saveSession(const SessionRecord& rec) {
    QSqlQuery query(m_db->database());
    query.prepare(
        "INSERT INTO sessions ("
        "title, created_at, llm_model_path, stt_model_path, "
        "embedding_model_path, system_prompt, max_context_length, "
        "enable_rag, transcription_language, auto_detect_language"
        ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
        );
    bindSessionFields(query, rec);
    if (!query.exec()) { qWarning() << query.lastError().text(); return -1; }
    return m_db->lastInsertId();
}

bool SessionRepository::deleteAll()
{
    QSqlQuery query(m_db->database());
    return query.exec("DELETE FROM sessions");
}

bool SessionRepository::updateSession(const SessionRecord& record){
    QSqlQuery query(m_db->database());
    query.prepare(
        "UPDATE sessions SET "
        "title = ?, created_at = ?, llm_model_path = ?, stt_model_path = ?, "
        "embedding_model_path = ?, system_prompt = ?, max_context_length = ?, "
        "enable_rag = ?, transcription_language = ?, auto_detect_language = ? "
        "WHERE id = ?"
        );

    bindSessionFields(query, record);
    query.addBindValue(record.dbId);

    return query.exec();
}

QList<SessionRecord> SessionRepository::loadAllSessions(){
    QList<SessionRecord> sessions;

    QSqlQuery query(m_db->database());
    query.exec(
        "SELECT id, title, created_at, llm_model_path, stt_model_path, "
        "embedding_model_path, system_prompt, max_context_length, "
        "enable_rag, transcription_language, auto_detect_language "
        "FROM sessions ORDER BY created_at DESC"
        );

    while (query.next()) {
        sessions.append(recordFromQuery(query));
    }

    return sessions;
}

SessionRecord SessionRepository::loadSession(int sessionId){
    QSqlQuery query(m_db->database());
    query.prepare(
        "SELECT id, title, created_at, llm_model_path, stt_model_path, "
        "embedding_model_path, system_prompt, max_context_length, "
        "enable_rag, transcription_language, auto_detect_language "
        "FROM sessions WHERE id = ?"
        );

    query.addBindValue(sessionId);

    return query.exec() && query.next() ? recordFromQuery(query) : SessionRecord();
}

bool SessionRepository::deleteSession(int sessionId)
{
    QSqlQuery query(m_db->database());
    query.prepare("DELETE FROM sessions WHERE id = ?");
    query.addBindValue(sessionId);
    return query.exec();
}

void SessionRepository::bindSessionFields(QSqlQuery& query, const SessionRecord& rec) {
    query.addBindValue(rec.title);
    query.addBindValue(rec.createdAt.toSecsSinceEpoch());
    query.addBindValue(rec.config.llmModelPath);
    query.addBindValue(rec.config.sttModelPath);
    query.addBindValue(rec.config.embModelPath);
    query.addBindValue(rec.config.systemPrompt);
    query.addBindValue(rec.config.contextLength);
    query.addBindValue(rec.config.enableRag ? 1 : 0);
    query.addBindValue(rec.config.language);
    query.addBindValue(rec.config.autoDetectLanguage ? 1 : 0);
}

SessionRecord SessionRepository::recordFromQuery(QSqlQuery& query) {
    SessionRecord rec;
    rec.dbId                        = query.value(0).toInt();
    rec.title                       = query.value(1).toString();
    rec.createdAt                   = QDateTime::fromSecsSinceEpoch(query.value(2).toLongLong());
    rec.config.llmModelPath         = query.value(3).toString();
    rec.config.sttModelPath         = query.value(4).toString();
    rec.config.embModelPath         = query.value(5).toString();
    rec.config.systemPrompt         = query.value(6).toString();
    rec.config.contextLength        = query.value(7).toInt();
    rec.config.enableRag            = query.value(8).toBool();
    rec.config.language             = query.value(9).toString();
    rec.config.autoDetectLanguage   = query.value(10).toBool();
    return rec;
}
