#include "liveTextify/modules/database/repositories/sessionrepository.h"
#include "liveTextify/modules/session/SessionRecord.h"
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
        "title, created_at, system_prompt"
        ") VALUES (?, ?, ?)"
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
        "title = ?, created_at = ?, system_prompt = ? WHERE id = ?"
        );

    bindSessionFields(query, record);
    query.addBindValue(record.dbId);

    return query.exec();
}

QList<SessionRecord> SessionRepository::loadAllSessions(){
    QList<SessionRecord> sessions;

    QSqlQuery query(m_db->database());
    query.exec(
        "SELECT id, title, system_prompt "
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
        "SELECT id, title, created_at, system_prompt "
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
    query.addBindValue(rec.systemPrompt);
}

SessionRecord SessionRepository::recordFromQuery(QSqlQuery& query) {
    SessionRecord rec;
    rec.dbId                        = query.value(0).toInt();
    rec.title                       = query.value(1).toString();
    rec.createdAt                   = QDateTime::fromSecsSinceEpoch(query.value(2).toLongLong());
    rec.systemPrompt                = query.value(3).toString();
    return rec;
}
