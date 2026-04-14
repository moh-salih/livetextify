#include "liveTextify/modules/settings/settingsmanager.h"

#include <QSettings>

SettingsManager::SettingsManager(QObject *parent): QObject(parent),
    mSTTConfigManager(new STTConfigManager(this)),
    mLLMConfigManager(new LLMConfigManager(this)),
    mAudioConfigManager(new AudioConfigManager(this)),
    mRAGConfigManager(new RAGConfigManager(this)),
    mEmbConfigManager(new EmbConfigManager(this))
{
    reload();
}

void SettingsManager::save(){
    if (mSTTConfigManager) {
        mSTTConfigManager->saveToSettings();
    }

    if(mLLMConfigManager){
        mLLMConfigManager->saveToSettings();
    }

    if(mAudioConfigManager){
        mAudioConfigManager->saveToSettings();
    }

    if(mRAGConfigManager){
        mRAGConfigManager->saveToSettings();
    }

    if(mEmbConfigManager){
        mEmbConfigManager->saveToSettings();
    }


}

void SettingsManager::reload(){
    if (mSTTConfigManager) {
        mSTTConfigManager->loadFromSettings();
    }

    if(mLLMConfigManager){
        mLLMConfigManager->loadFromSettings();
    }

    if(mAudioConfigManager){
        mAudioConfigManager->loadFromSettings();
    }


    if(mRAGConfigManager){
        mRAGConfigManager->loadFromSettings();
    }

    if(mEmbConfigManager){
        mEmbConfigManager->loadFromSettings();
    }
}
