#include "sessionservice.h"
#include "liveTextify/modules/services/databaseservice.h"
#include "liveTextify/modules/settings/settingsmanager.h"
#include "liveTextify/core/Logger.h"

SessionService::SessionService(SettingsManager * settings, DatabaseService * database, QObject* parent)
    : QObject(parent)
    , mLLMConfigManager(settings->llmConfigManager())
    , mSTTConfigManager(settings->sttConfigManager())
    , mEmbConfigManager(settings->embConfigManager())
    , mDatabaseService(database)
    , mSessionsModel(new SessionModel(this)){
}

SessionService::~SessionService() = default;

void SessionService::loadHistory() {
    QList<SessionRecord> records = mDatabaseService->loadAllSessions();

    for (int i = records.size() - 1; i >= 0; --i) {
        Session* session = sessionFromRecord(records[i]);

        auto msgs = mDatabaseService->loadMessages(session->sessionID());
        foreach (const auto& msg , msgs) session->conversation()->addMessage(msg);
        session->setTranscription(mDatabaseService->loadTranscription(session->sessionID()));

        setupSessionConnections(session);
        mSessionsModel->addSession(session);
    }
    Logger::info(QString("SessionService: Loaded %1 sessions from disk.").arg(mSessionsModel->rowCount()));
}
Session* SessionService::createSession(const QVariantMap& map) {
    closeSession();

    SessionRecord rec;
    rec.createdAt = QDateTime::currentDateTime();

    // Seed from global settings first
    const auto llmConfig          = mLLMConfigManager->config();
    rec.config.systemPrompt       = llmConfig.systemPrompt;
    rec.config.contextLength      = llmConfig.contextLength;
    rec.config.maxTokens          = llmConfig.maxTokens;
    rec.config.temperature        = llmConfig.temperature;

    const auto sttConfig          = mSTTConfigManager->config();
    rec.config.language           = sttConfig.language;
    rec.config.autoDetectLanguage = sttConfig.autoDetectLanguage;
    rec.config.translate          = sttConfig.translate;
    rec.config.enableRag          = map.value("enableRag", true).toBool();

    // Apply map overrides on top
    rec.title                     = map.value("title").toString();
    rec.config.sttModelPath       = map.value("sttModelPath").toString();
    rec.config.llmModelPath       = map.value("llmModelPath").toString();
    rec.config.embModelPath       = map.value("embeddingModelPath").toString();

    if (map.contains("systemPrompt"))
        rec.config.systemPrompt   = map.value("systemPrompt").toString();
    if (map.contains("enableRag"))
        rec.config.enableRag      = map.value("enableRag").toBool();

    int dbId = mDatabaseService->saveSession(rec);
    if (dbId == -1) return nullptr;

    rec.dbId = dbId;
    Session* session = sessionFromRecord(rec);
    setupSessionConnections(session);
    mSessionsModel->addSession(session);
    emit sessionCreated(mSessionsModel->rowCount() - 1);
    openSessionById(dbId);
    return session;
}

bool SessionService::openSessionById(int sessionId) {
    Session* target = nullptr;
    for (int i = 0; i < mSessionsModel->rowCount(); ++i) {
        if (mSessionsModel->get(i)->sessionID() == sessionId) {
            target = mSessionsModel->get(i);
            break;
        }
    }

    if (!target || target == mActiveSession) return false;

    // Guarantees old models are dumped before loading new ones
    closeSession();

    mActiveSession = target;
    mSessionsModel->setCurrentSession(target); // Sync UI model highlight

    emit activeSessionChanged(mActiveSession);
    Logger::info(QString("SessionService: Active session is now %1").arg(sessionId));
    return true;
}

void SessionService::closeSession() {
    if (!mActiveSession) return;
    mActiveSession = nullptr;
    mSessionsModel->setCurrentSession(nullptr);

    // This broadcast hits AiService and forces memory to empty
    emit activeSessionChanged(mActiveSession);
}

bool SessionService::deleteSessionById(int sessionId) {
    for (int i = 0; i < mSessionsModel->rowCount(); ++i) {
        if (mSessionsModel->get(i)->sessionID() == sessionId) return deleteSession(i);
    }
    return false;
}

bool SessionService::deleteSession(int index) {
    Session* session = mSessionsModel->get(index);
    if (!session) return false;

    int id = session->sessionID();
    if (session == mActiveSession) closeSession();

    mSessionsModel->removeSession(session);
    mDatabaseService->deleteSession(id); // Cascade delete in DB handles messages/transcription

    session->deleteLater();
    emit sessionDeleted(index);
    return true;
}

void SessionService::deleteAll(){
    Logger::warning("SessionService: Wiping all user history data.");

    closeSession();

    if (mDatabaseService->deleteAllSessions()) {
        mSessionsModel->clear();
        Logger::info("SessionService: All sessions purged successfully.");
    } else {
        Logger::error("SessionService: Failed to purge database records.");
    }
}

void SessionService::setupSessionConnections(Session* session) {
    connect(session, &Session::transcriptionChanged, this, &SessionService::onSessionTranscriptionChanged);
    connect(session, &Session::userMessageReady, this, &SessionService::onSessionUserMessageReady);
    connect(session, &Session::titleChanged, this, [this, session]() {
        SessionRecord rec;
        rec.dbId = session->sessionID();
        rec.title = session->title();
        rec.createdAt = session->createdAt();
        rec.config = session->config();

        mDatabaseService->updateSession(rec);
        Logger::debug(QString("SessionService: Persisted update for session %1").arg(rec.dbId));
    });
}

Session* SessionService::sessionFromRecord(const SessionRecord& rec) {
    Session* session = new Session(this);
    session->setDbId(rec.dbId);
    session->setTitle(rec.title);
    session->setCreatedAt(rec.createdAt);
    session->config() = rec.config;
    return session;
}

void SessionService::onSessionTranscriptionChanged() {
    Session* s = qobject_cast<Session*>(sender());
    if (s) emit transcriptionUpdated(s->transcription(), s->sessionID());
}

void SessionService::onSessionUserMessageReady(const QString& text, int sessionId) {
    emit userMessageReady(text, sessionId);
}

Session* SessionService::getSession(int index) const {
    return mSessionsModel->get(index);
}
