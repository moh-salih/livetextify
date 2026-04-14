// C:\Users\salih\code\staj\livetextify\src\livetextify\modules\database\messagerepository.h
#ifndef MESSAGEREPOSITORY_H
#define MESSAGEREPOSITORY_H

#include <QObject>
#include <QList>
#include "liveTextify/modules/chat/Message.h"

class DatabaseManager;

class MessageRepository : public QObject
{
    Q_OBJECT
public:
    explicit MessageRepository(DatabaseManager* db, QObject *parent = nullptr);

    bool saveMessage(int sessionId, const Message& message, int index);
    bool saveMessages(int sessionId, const QVector<Message>& messages);
    QVector<Message> loadMessages(int sessionId);
    bool deleteMessages(int sessionId);
    int getMessageCount(int sessionId);

private:
    DatabaseManager* m_db;
};

#endif // MESSAGEREPOSITORY_H
