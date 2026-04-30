#include "liveTextify/modules/settings/settingsmanager.h"
#include "liveTextify/modules/session/session.h"

#include <QSettings>

SettingsManager::SettingsManager(QObject *parent): QObject(parent),
    mSTTConfigManager(new STTConfigManager(this)),
    mLLMConfigManager(new LLMConfigManager(this)),
    mAudioConfigManager(new AudioConfigManager(this)),
    mRAGConfigManager(new RAGConfigManager(this)),
    mEmbConfigManager(new EmbConfigManager(this))
{
    auto onAnyChanged = [this]() {
        if (!mDiscarding && !mHasUnsavedChanges) {
            mHasUnsavedChanges = true;
            emit hasUnsavedChangesChanged();
        }
        // always emit configChanged regardless — active session still needs updating
        SessionConfig cfg;
        cfg.stt          = mSTTConfigManager->config();
        cfg.llm          = mLLMConfigManager->config();
        cfg.emb          = mEmbConfigManager->config();
        cfg.rag          = mRAGConfigManager->config();
        cfg.audio        = mAudioConfigManager->config();
        cfg.enableRag    = mRAGConfigManager->enabled();
        cfg.systemPrompt = mLLMConfigManager->systemPrompt();
        emit configChanged(cfg);
    };

    connect(mSTTConfigManager,      &STTConfigManager::configChanged,   this, onAnyChanged);
    connect(mLLMConfigManager,      &LLMConfigManager::configChanged,   this, onAnyChanged);
    connect(mAudioConfigManager,    &AudioConfigManager::configChanged, this, onAnyChanged);
    connect(mRAGConfigManager,      &RAGConfigManager::configChanged,   this, onAnyChanged);
    connect(mEmbConfigManager,      &EmbConfigManager::configChanged,   this, onAnyChanged);
}

void SettingsManager::save(){
    mSTTConfigManager->saveToSettings();
    mLLMConfigManager->saveToSettings();
    mAudioConfigManager->saveToSettings();
    mRAGConfigManager->saveToSettings();
    mEmbConfigManager->saveToSettings();

    mHasUnsavedChanges = false;
    emit hasUnsavedChangesChanged();
}

void SettingsManager::reload(){
    mSTTConfigManager->loadFromSettings();
    mLLMConfigManager->loadFromSettings();
    mAudioConfigManager->loadFromSettings();
    mRAGConfigManager->loadFromSettings();
    mEmbConfigManager->loadFromSettings();

    SessionConfig cfg;
    cfg.stt          = mSTTConfigManager->config();
    cfg.llm          = mLLMConfigManager->config();
    cfg.emb          = mEmbConfigManager->config();
    cfg.rag          = mRAGConfigManager->config();
    cfg.audio        = mAudioConfigManager->config();
    cfg.enableRag    = mRAGConfigManager->enabled();
    cfg.systemPrompt = mLLMConfigManager->systemPrompt();
    emit configChanged(cfg);
}

void SettingsManager::discard() {
    mHasUnsavedChanges = false;
    mDiscarding = true;
    reload();
    mDiscarding = false;
    emit hasUnsavedChangesChanged();
}

void SettingsManager::onDefaultModelChanged(const QString& type, const QString& path) {
    mDiscarding = true;
    if      (type == "stt") mSTTConfigManager->setModelPath(path);
    else if (type == "llm") mLLMConfigManager->setModelPath(path);
    else if (type == "emb") mEmbConfigManager->setModelPath(path);
    mDiscarding = false;

    // Persist immediately
    if      (type == "stt") mSTTConfigManager->saveToSettings();
    else if (type == "llm") mLLMConfigManager->saveToSettings();
    else if (type == "emb") mEmbConfigManager->saveToSettings();

    // Emit configChanged so SessionService picks up the new path
    SessionConfig cfg;
    cfg.stt          = mSTTConfigManager->config();
    cfg.llm          = mLLMConfigManager->config();
    cfg.emb          = mEmbConfigManager->config();
    cfg.rag          = mRAGConfigManager->config();
    cfg.audio        = mAudioConfigManager->config();
    cfg.enableRag    = mRAGConfigManager->enabled();
    cfg.systemPrompt = mLLMConfigManager->systemPrompt();
    emit configChanged(cfg);
}
