#include "liveTextify/modules/database/repositories/transcriptionrepository.h"
#include "liveTextify/modules/database/databasemanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>

TranscriptionRepository::TranscriptionRepository(DatabaseManager* db, QObject *parent)
    : QObject(parent)
    , m_db(db)
{
}

bool TranscriptionRepository::saveTranscription(int sessionId, const QString& text)
{
    QSqlQuery query(m_db->database());
    query.prepare(
        "INSERT OR REPLACE INTO transcriptions (session_id, text, updated_at) "
        "VALUES (?, ?, ?)"
        );

    query.addBindValue(sessionId);
    query.addBindValue(text);
    query.addBindValue(QDateTime::currentSecsSinceEpoch());

    return query.exec();
}

QString TranscriptionRepository::loadTranscription(int sessionId)
{
    QSqlQuery query(m_db->database());
    query.prepare("SELECT text FROM transcriptions WHERE session_id = ?");
    query.addBindValue(sessionId);

    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }

    return QString();
}

bool TranscriptionRepository::updateTranscription(int sessionId, const QString& text)
{
    return saveTranscription(sessionId, text); // INSERT OR REPLACE handles this
}

bool TranscriptionRepository::deleteTranscription(int sessionId)
{
    QSqlQuery query(m_db->database());
    query.prepare("DELETE FROM transcriptions WHERE session_id = ?");
    query.addBindValue(sessionId);
    return query.exec();
}
