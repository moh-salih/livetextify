// C:\Users\salih\code\staj\livetextify\src\livetextify\modules\database\transcriptionrepository.h
#pragma once
#include <QObject>
#include <QString>

class DatabaseManager;

class TranscriptionRepository : public QObject
{
    Q_OBJECT
public:
    explicit TranscriptionRepository(DatabaseManager* db, QObject *parent = nullptr);

    bool saveTranscription(int sessionId, const QString& text);
    QString loadTranscription(int sessionId);
    bool updateTranscription(int sessionId, const QString& text);
    bool deleteTranscription(int sessionId);

private:
    DatabaseManager* m_db;
};

