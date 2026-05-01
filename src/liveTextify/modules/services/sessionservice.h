#pragma once

#include <QObject>
#include <QVariantMap>
#include "liveTextify/modules/session/session.h"
#include "liveTextify/modules/session/sessionmodel.h"
#include "liveTextify/modules/session/sessionconfigmanager.h"

class DatabaseService;
struct SessionRecord;

class SessionService : public QObject {
    Q_OBJECT
    Q_PROPERTY(SessionModel* sessions      READ sessions      CONSTANT)
    Q_PROPERTY(Session* activeSession READ activeSession NOTIFY activeSessionChanged)
    Q_PROPERTY(SessionConfigManager* sessionConfig READ configManager NOTIFY activeSessionChanged)
public:
    explicit SessionService(DatabaseService* database, QObject* parent = nullptr);
    ~SessionService() override;

    Q_INVOKABLE Session *createSession();
    Q_INVOKABLE bool     openSessionById(int sessionId);
    Q_INVOKABLE bool     deleteSession(int index);
    Q_INVOKABLE bool     deleteSessionById(int sessionId);
    Q_INVOKABLE void     deleteAll();
    Q_INVOKABLE void     closeSession();

    Q_INVOKABLE void sendMessage(const QString& text);
    Q_INVOKABLE void stopGenerating();

    void loadHistory();

    SessionModel* sessions()      const { return mSessionsModel; }
    Session* activeSession() const { return mActiveSession; }
    Session* getSession(int index) const;
    SessionConfigManager* configManager() const { return mConfigManager; }

public slots:
    void onGlobalConfigsUpdated(const SessionConfig& config);
    void onTranscriptionUpdated(const QString& text);

private slots:
    void onActiveSessionChanged(Session* session);
    void onConfigChanged();
signals:
    void sessionCreated(int index);
    void sessionDeleted(int index);
    void activeSessionChanged(Session* session);
    void activeSessionConfigChanged(const SessionConfig& config);

    // Consumed by TranscriptionService / ChatService
    void transcriptionUpdated(const QString& text, int sessionId);
    void userMessageReady(const QString& text, int sessionId);
    void configManagerChanged(); // emitted when active session switches
    void stopGenerationRequested();

private:
    Session* sessionFromRecord(const SessionRecord& rec);
    void     persistSession(Session* session);

    SessionModel* mSessionsModel;
    Session* mActiveSession = nullptr;
    DatabaseService* mDatabaseService;
    SessionConfigManager* mConfigManager = nullptr;
    SessionConfig           mCurrentGlobalConfig;
};
