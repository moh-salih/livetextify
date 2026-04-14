#pragma once

#include <QObject>
#include <QVariantMap>
#include "liveTextify/modules/session/session.h"
#include "liveTextify/modules/session/sessionmodel.h"

class DatabaseService;
class SettingsManager;
class STTConfigManager;
class LLMConfigManager;
class EmbConfigManager;

class SessionService : public QObject {
    Q_OBJECT
    Q_PROPERTY(SessionModel* sessions READ sessions CONSTANT)
    Q_PROPERTY(Session* activeSession READ activeSession NOTIFY activeSessionChanged)

public:
    explicit SessionService(SettingsManager * settings, DatabaseService * database, QObject* parent = nullptr);
    ~SessionService() override;

    // --- CRUD Operations (Invokable from UI/Manager) ---
    Q_INVOKABLE Session* createSession(const QVariantMap& configMap);
    Q_INVOKABLE bool openSessionById(int sessionId);
    Q_INVOKABLE bool deleteSession(int index);
    Q_INVOKABLE void deleteAll();
    Q_INVOKABLE bool deleteSessionById(int sessionId);
    Q_INVOKABLE void closeSession();

    // --- Data Loading ---
    void loadHistory();

    // --- Getters ---
    SessionModel* sessions() const { return mSessionsModel; }
    Session* activeSession() const { return mActiveSession; }

    Session* getSession(int index) const;

signals:
    void sessionCreated(int index);
    void sessionDeleted(int index);
    void activeSessionChanged(Session* session);

    // Global signals that logic services (RAG/Conversation) listen to
    void transcriptionUpdated(const QString& text, int sessionId);
    void userMessageReady(const QString& text, int sessionId);

private slots:
    // Internal signal mapping from Session objects to Global Service signals
    void onSessionTranscriptionChanged();
    void onSessionUserMessageReady(const QString& text, int sessionId);

private:
    void setupSessionConnections(Session* session);
    Session* sessionFromRecord(const SessionRecord& rec);

    // Models & State
    SessionModel        * mSessionsModel;
    Session             * mActiveSession = nullptr;
    DatabaseService     * mDatabaseService;

    STTConfigManager    * mSTTConfigManager;
    LLMConfigManager    * mLLMConfigManager;
    EmbConfigManager    * mEmbConfigManager;
};
