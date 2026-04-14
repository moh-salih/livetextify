#pragma once
#include <QObject>
#include <QList>
#include <QSqlQuery>


class DatabaseManager;
struct SessionRecord;

class SessionRepository : public QObject {
    Q_OBJECT
public:
    explicit SessionRepository(DatabaseManager* db, QObject *parent = nullptr);

    int saveSession(const SessionRecord& rec);
    bool updateSession(const SessionRecord& rec);
    bool deleteSession(int sessionId);
    bool deleteAll();
    QList<SessionRecord> loadAllSessions();
    SessionRecord loadSession(int sessionId);

private:
    void bindSessionFields(QSqlQuery& query, const SessionRecord& rec);
    SessionRecord recordFromQuery(QSqlQuery& query);
    DatabaseManager* m_db;
};
