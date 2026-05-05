#pragma once

#include <QObject>
#include <QVariantMap>
#include "liveTextify/modules/session/session.h"
#include "liveTextify/modules/session/sessionmodel.h"
#include "liveTextify/modules/session/sessionsettings.h"

class DatabaseService;
struct SessionRecord;

class SessionService : public QObject {
    Q_OBJECT
    Q_PROPERTY(SessionModel*    sessions        READ sessions        CONSTANT)
    Q_PROPERTY(Session*         activeSession   READ activeSession   NOTIFY activeSessionChanged)
    Q_PROPERTY(SessionSettings* settings        READ settings        CONSTANT)

public:
    explicit SessionService(DatabaseService* database, QObject* parent = nullptr);
    ~SessionService() override;

    Q_INVOKABLE Session* createSession(const QString& title = "New Session");
    Q_INVOKABLE bool     openSessionById(int sessionId);
    Q_INVOKABLE bool     deleteSession(int index);
    Q_INVOKABLE bool     deleteSessionById(int sessionId);
    Q_INVOKABLE void     deleteAll();
    Q_INVOKABLE void     closeSession();

    Q_INVOKABLE void    sendMessage(const QString& text);
    Q_INVOKABLE void    stopGenerating();

    void loadHistory();

    SessionModel*    sessions()     const { return mSessionsModel; }
    Session*         activeSession() const { return mActiveSession; }
    Session*         getSession(int index) const;
    SessionSettings* settings()     const { return mSettings; }

public slots:
    void onTranscriptionUpdated(const QString& text);

signals:
    void sessionCreated(int index);
    void sessionDeleted(int index);
    void activeSessionChanged(Session* session);

    // Consumed by TranscriptionService / ChatService
    void transcriptionUpdated(const QString& text, int sessionId);
    void userMessageReady(const QString& text, int sessionId);
    void stopGenerationRequested();

private slots:
    void onActiveSessionChanged(Session* session);
    void onSettingsChanged(const SessionConfig& config);

private:
    Session* sessionFromRecord(const SessionRecord& rec);
    void     persistSession(Session* session);

    SessionModel*    mSessionsModel;
    Session*         mActiveSession = nullptr;
    DatabaseService* mDatabaseService;
    SessionSettings* mSettings;
};
