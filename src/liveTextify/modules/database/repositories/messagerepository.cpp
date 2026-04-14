#include "liveTextify/modules/database/repositories/messagerepository.h"
#include "liveTextify/modules/database/databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>

MessageRepository::MessageRepository(DatabaseManager* db, QObject *parent)
    : QObject(parent)
    , m_db(db)
{
}

bool MessageRepository::saveMessage(int sessionId, const Message& message, int index)
{
    QSqlQuery query(m_db->database());
    query.prepare(
        "INSERT INTO messages (session_id, role, content, timestamp, message_index) "
        "VALUES (?, ?, ?, ?, ?)"
        );

    query.addBindValue(sessionId);
    query.addBindValue(message.role);
    query.addBindValue(message.content);
    query.addBindValue(message.timestamp.toSecsSinceEpoch());
    query.addBindValue(index);

    return query.exec();
}

bool MessageRepository::saveMessages(int sessionId, const QVector<Message>& messages)
{
    if (messages.isEmpty()) return true;

    if (!m_db->beginTransaction()) return false;

    // First delete existing messages for this session (avoid duplicates)
    deleteMessages(sessionId);

    for (int i = 0; i < messages.size(); ++i) {
        if (!saveMessage(sessionId, messages[i], i)) {
            m_db->rollbackTransaction();
            return false;
        }
    }

    return m_db->commitTransaction();
}

QVector<Message> MessageRepository::loadMessages(int sessionId)
{
    QVector<Message> messages;

    QSqlQuery query(m_db->database());
    query.prepare(
        "SELECT role, content, timestamp, message_index FROM messages "
        "WHERE session_id = ? ORDER BY message_index ASC"
        );
    query.addBindValue(sessionId);

    if (query.exec()) {
        while (query.next()) {
            Message msg;
            msg.role = query.value(0).toString();
            msg.content = query.value(1).toString();
            msg.timestamp = QDateTime::fromSecsSinceEpoch(query.value(2).toLongLong());
            msg.isComplete = true; // All saved messages are complete
            messages.append(msg);
        }
    }

    return messages;
}

bool MessageRepository::deleteMessages(int sessionId)
{
    QSqlQuery query(m_db->database());
    query.prepare("DELETE FROM messages WHERE session_id = ?");
    query.addBindValue(sessionId);
    return query.exec();
}

int MessageRepository::getMessageCount(int sessionId)
{
    QSqlQuery query(m_db->database());
    query.prepare("SELECT COUNT(*) FROM messages WHERE session_id = ?");
    query.addBindValue(sessionId);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}
