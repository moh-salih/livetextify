#include "sessionservice.h"
#include "liveTextify/modules/services/databaseservice.h"
#include "liveTextify/modules/session/sessionconfigmanager.h"
#include "liveTextify/modules/settings/settingsmanager.h"
#include "liveTextify/modules/session/SessionRecord.h"
#include "liveTextify/core/Logger.h"

SessionService::SessionService(DatabaseService* database, QObject* parent)
    : QObject(parent)
    , mSessionsModel(new SessionModel(this))
    , mDatabaseService(database)
{
    mConfigManager = new SessionConfigManager(this);
    connect(mConfigManager, &SessionConfigManager::configChanged, this, &SessionService::onConfigChanged);
    connect(this, &SessionService::activeSessionChanged, this, &SessionService::onActiveSessionChanged);
}

SessionService::~SessionService() = default;

void SessionService::onTranscriptionUpdated(const QString& text) {
    if (!mActiveSession) return;

    // Accumulate instead of replace
    QString accumulated = mActiveSession->transcription();
    if (!accumulated.isEmpty()) accumulated += " ";
    accumulated += text;

    mDatabaseService->saveTranscription(mActiveSession->sessionID(), accumulated);
    mActiveSession->setTranscription(accumulated);
    emit transcriptionUpdated(accumulated, mActiveSession->sessionID());
}

void SessionService::onGlobalConfigsUpdated(const SessionConfig& config) {
    mCurrentGlobalConfig = config;
    Logger::info("Updating Session Config Manager");
    if (!mActiveSession || !mConfigManager) return;

    Logger::info("Updated Session Config Manager");
    mActiveSession->setConfig(config);
    mConfigManager->seedFrom(config);
}

void SessionService::sendMessage(const QString& text) {
    if (!mActiveSession || text.trimmed().isEmpty()) return;
    emit userMessageReady(text, mActiveSession->sessionID());
}

void SessionService::stopGenerating() {
    emit stopGenerationRequested();
}

// ── History ───────────────────────────────────────────────────────────────────

void SessionService::loadHistory() {
    const QList<SessionRecord> records = mDatabaseService->loadAllSessions();

    for (int i = records.size() - 1; i >= 0; --i) {
        Session* session = sessionFromRecord(records[i]);  // already has conversation

        for (const auto& msg : mDatabaseService->loadMessages(session->sessionID()))
            session->conversation()->addMessage(msg);  // safe now

        session->setTranscription(mDatabaseService->loadTranscription(session->sessionID()));
        mSessionsModel->addSession(session);
    }

    Logger::info(QString("SessionService: Loaded %1 sessions from disk.").arg(mSessionsModel->rowCount()));
}

// ── CRUD ──────────────────────────────────────────────────────────────────────
Session* SessionService::createSession() {
    Session* session = new Session(this);
    session->setConfig(mCurrentGlobalConfig);

    SessionRecord rec;
    rec.title     = "New Session";
    rec.createdAt = QDateTime::currentDateTime();

    const int dbId = mDatabaseService->saveSession(rec);
    if (dbId == -1) {
        Logger::error("SessionService: Failed to persist new session.");
        delete session;
        return nullptr;
    }

    session->setDbId(dbId);  // ← must be set before addSession
    session->loadConversation({});

    Logger::info("Creating session with models: \n\t" +
                 session->config().stt.modelPath + "\n\t" +
                 session->config().llm.modelPath + "\n\t" +
                 session->config().emb.modelPath);

    mSessionsModel->addSession(session);
    emit sessionCreated(mSessionsModel->rowCount() - 1);

    openSessionById(dbId);
    return session;
}

bool SessionService::openSessionById(int sessionId) {
    closeSession();
    Logger::info("[SessionService::openSessionById] Opening session: " + QString::number(sessionId));

    Session* target = nullptr;
    for (int i = 0; i < mSessionsModel->rowCount(); ++i) {
        if (mSessionsModel->get(i)->sessionID() == sessionId) {
            target = mSessionsModel->get(i);
            break;
        }
    }

    if (!target || target == mActiveSession) return false;

    mActiveSession = target;
    mSessionsModel->setCurrentSession(target);

    emit activeSessionChanged(mActiveSession);
    Logger::info(QString("SessionService: Active session → %1").arg(sessionId));
    return true;
}

void SessionService::closeSession() {
    if (!mActiveSession) return;
    mActiveSession = nullptr;
    mSessionsModel->setCurrentSession(nullptr);
    emit activeSessionChanged(nullptr);
}

bool SessionService::deleteSessionById(int sessionId) {
    for (int i = 0; i < mSessionsModel->rowCount(); ++i) {
        if (mSessionsModel->get(i)->sessionID() == sessionId)
            return deleteSession(i);
    }
    return false;
}

bool SessionService::deleteSession(int index) {
    Session* session = mSessionsModel->get(index);
    if (!session) return false;

    const int id = session->sessionID();
    if (session == mActiveSession) closeSession();

    mSessionsModel->removeSession(session);
    mDatabaseService->deleteSession(id);

    session->deleteLater();
    emit sessionDeleted(index);
    return true;
}

void SessionService::deleteAll() {
    Logger::warning("SessionService: Wiping all user history.");
    closeSession();

    if (mDatabaseService->deleteAllSessions()) {
        mSessionsModel->clear();
        Logger::info("SessionService: All sessions purged.");
    } else {
        Logger::error("SessionService: Failed to purge database.");
    }
}

// ── Internal ──────────────────────────────────────────────────────────────────

Session* SessionService::sessionFromRecord(const SessionRecord& rec) {
    Session* session = new Session(this);
    session->setDbId(rec.dbId);
    session->setTitle(rec.title);
    session->setCreatedAt(rec.createdAt);
    session->loadConversation({});
    return session;
}

void SessionService::persistSession(Session* session) {
    SessionRecord rec;
    rec.dbId      = session->sessionID();
    rec.title     = session->title();
    rec.createdAt = session->createdAt();
    mDatabaseService->updateSession(rec);
    Logger::debug(QString("SessionService: Persisted session %1").arg(rec.dbId));
}

Session* SessionService::getSession(int index) const {
    return mSessionsModel->get(index);
}
