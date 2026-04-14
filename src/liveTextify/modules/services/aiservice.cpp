#include "aiservice.h"
#include "liveTextify/modules/session/session.h"
#include "liveTextify/modules/engines/stt/sttsession.h"
#include "liveTextify/modules/engines/llm/llmsession.h"
#include "liveTextify/modules/engines/emb/embsession.h"
#include "liveTextify/core/Logger.h"

// Engines
#include "liveTextify/modules/engines/stt/whispersttengine.h"
#include "liveTextify/modules/engines/llm/llamallmengine.h"
#include "liveTextify/modules/engines/emb/llamaembeddingengine.h"

#include "liveTextify/modules/settings/settingsmanager.h"


AiService::AiService(SettingsManager * settings, QObject* parent):
    QObject(parent),
    mSttSession(new SttSession(this)),
    mLlmSession(new LlmSession(this)),
    mEmbSession(new EmbSession(this)),
    mSTTConfigManager(settings->sttConfigManager()),
    mLLMConfigManager(settings->llmConfigManager()),
    mEmbConfigManager(settings->embConfigManager())
{

    // Inject hardware worker engines
    mSttSession->initialize(new WhisperSttEngine(this));
    mLlmSession->initialize(new LlamaLlmEngine(this));
    mEmbSession->initialize(new LlamaEmbeddingEngine(this));
}

AiService::~AiService() = default;

void AiService::processAudioWindow(const std::vector<float>& samples) {
    mSttSession->processAudioWindow(samples);
}


void AiService::onActiveSessionChanged(Session* activeSession) {
    if (!activeSession) {
        unloadModels();
        return;
    }

    Logger::info(QString("AiService: Active session changed to %1").arg(activeSession->sessionID()));
    loadModels(activeSession);
}


void AiService::loadModels(Session* activeSession) {
    if (!activeSession) {
        unloadModels();
        return;
    }


    Logger::info(QString("AiService: Loading models for session %1").arg(activeSession->sessionID()));

    const SessionConfig& sessionConfig = activeSession->config();
    STTConfig sttConfig             = mSTTConfigManager->config();
    sttConfig.modelPath             = sessionConfig.sttModelPath;
    sttConfig.language              = sessionConfig.language;
    sttConfig.autoDetectLanguage    = sessionConfig.autoDetectLanguage;
    sttConfig.translate             = sessionConfig.translate;

    LLMConfig llmConfig             = mLLMConfigManager->config();
    llmConfig.modelPath             = sessionConfig.llmModelPath;
    llmConfig.systemPrompt          = sessionConfig.systemPrompt;
    llmConfig.contextLength         = sessionConfig.contextLength;
    llmConfig.maxTokens             = sessionConfig.maxTokens;
    llmConfig.temperature           = sessionConfig.temperature;

    EmbConfig embConfig             = mEmbConfigManager->config();
    embConfig.modelPath             = sessionConfig.embModelPath;


    mSttSession->setConfig(sttConfig);
    mLlmSession->setConfig(llmConfig);
    mEmbSession->setConfig(embConfig);


    mSttSession->loadModel();
    mLlmSession->loadModel();
    mEmbSession->loadModel();
}

void AiService::unloadModels() {
    Logger::info("AiService: Unloading all AI models from memory");
    mSttSession->unloadModel();
    mLlmSession->unloadModel();
    mEmbSession->unloadModel();
}
