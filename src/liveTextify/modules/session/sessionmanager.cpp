#include "sessionmanager.h"

#include "liveTextify/modules/services/sessionservice.h"
#include "liveTextify/modules/services/aiservice.h"
#include "liveTextify/modules/services/conversationservice.h"
#include "liveTextify/modules/services/ragservice.h"
#include "liveTextify/modules/services/questionservice.h"
#include "liveTextify/modules/services/audioservice.h"
#include "liveTextify/modules/services/databaseservice.h"

SessionManager::SessionManager(SettingsManager * settings, QObject *parent) : QObject(parent) {
    mDatabaseService        = new DatabaseService(this);
    mSessionService         = new SessionService(settings, mDatabaseService, this);
    mAiService              = new AiService(settings, this);
    mAudioService           = new AudioService(settings, this);
    mRagService             = new RagService(settings, mDatabaseService, mAiService, mSessionService, this);
    mConversationService    = new ConversationService(settings, mDatabaseService, mAiService, mSessionService, this);
    mQuestionService        = new QuestionService(mAiService, mSessionService, this);

    // Connect cross-service signals
    connect(mSessionService,      &SessionService::activeSessionChanged,    mAiService,             &AiService::onActiveSessionChanged);
    connect(mConversationService, &ConversationService::requestContext,     mRagService,            &RagService::requestContext);
    connect(mRagService,          &RagService::contextReady,                mConversationService,   &ConversationService::onContextReady);
    connect(mSessionService,      &SessionService::userMessageReady,        mConversationService,   &ConversationService::handleUserMessage);
    connect(mAudioService,        &AudioService::windowReady,               mAiService,             &AiService::processAudioWindow);

    mSessionService->loadHistory();
}

SessionManager::~SessionManager() = default;
