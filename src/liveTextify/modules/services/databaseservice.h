#pragma once
#include <QObject>

class SessionRepository;
class MessageRepository;
class TranscriptionRepository;
class ChunkRepository;
class DatabaseManager;
struct SessionRecord;
struct Message;
struct ChunkData;

class DatabaseService : public QObject {
    Q_OBJECT
public:
    explicit DatabaseService(QObject* parent = nullptr);

    // Public API — services call these, never touch repositories directly
    int  saveSession(const SessionRecord& rec);
    bool updateSession(const SessionRecord& rec);
    bool deleteSession(int sessionId);
    bool deleteAllSessions();
    QList<SessionRecord> loadAllSessions();

    bool saveMessage(int sessionId, const Message& msg, int index);
    bool saveMessages(int sessionId, const QVector<Message>& messages);
    QVector<Message> loadMessages(int sessionId);
    bool deleteMessages(int sessionId);

    bool    saveTranscription(int sessionId, const QString& text);
    QString loadTranscription(int sessionId);
    bool    deleteTranscription(int sessionId);

    bool saveChunk(int sessionId, const QString& text, int index, const std::vector<float>& embedding);
    QVector<ChunkData> loadChunks(int sessionId);
    bool deleteChunks(int sessionId);
    int  getChunkCount(int sessionId);

private:
    SessionRepository       * mSessionRepo;
    MessageRepository       * mMessageRepo;
    TranscriptionRepository * mTranscriptionRepo;
    ChunkRepository         * mChunkRepo;
};
