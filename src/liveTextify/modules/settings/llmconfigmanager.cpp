#include "liveTextify/modules/settings/llmconfigmanager.h"
#include "liveTextify/core/Logger.h"
#include <QSettings>

LLMConfigManager::LLMConfigManager(QObject * parent): QObject(parent){

}

void LLMConfigManager::setSystemPrompt(const QString & value){
    if(mSystemPrompt == value) return;
    mSystemPrompt = value;

    emit systemPromptChanged();
}

void LLMConfigManager::setThreadCount(int value){
    if(mConfig.threadCount == value) return;
    mConfig.threadCount = value;

    emit threadCountChanged();
}

void LLMConfigManager::setContextLength(int value){
    if(mConfig.contextLength == value) return;
    mConfig.contextLength = value;

    emit contextLengthChanged();
}

void LLMConfigManager::setBatchSize(int value){
    if(mConfig.batchSize == value) return;
    mConfig.batchSize = value;

    emit batchSizeChanged();
}

void LLMConfigManager::setMaxTokens(int value){
    if(mConfig.maxTokens == value) return;
    mConfig.maxTokens = value;

    emit maxTokensChanged();
}

void LLMConfigManager::setTopK(int value){
    if(mConfig.topK == value) return;
    mConfig.topK = value;

    emit topKChanged();
}

void LLMConfigManager::setBatchThreads(int value){
    if(mConfig.batchThreads == value) return;
    mConfig.batchThreads = value;

    emit batchThreadsChanged();
}

void LLMConfigManager::setTemperature(qreal value){
    if(qFuzzyCompare(mConfig.temperature, static_cast<float>(value))) return;
    mConfig.temperature = value;

    emit temperatureChanged();
}

void LLMConfigManager::setTopP(qreal value){
    if(qFuzzyCompare(mConfig.topP, static_cast<float>(value))) return;
    mConfig.topP = value;

    emit topPChanged();
}

void LLMConfigManager::setRepeatPenalty(qreal value){
    if(qFuzzyCompare(mConfig.repeatPenalty, static_cast<float>(value))) return;
    mConfig.repeatPenalty = value;

    emit repeatPenaltyChanged();
}

void LLMConfigManager::setModelPath(const QString& v) {
    if (mConfig.modelPath == v) return;
    mConfig.modelPath = v;
    emit configChanged();
}


void LLMConfigManager::resetToDefaults(){
    mConfig = LLMConfig();

    emit systemPromptChanged();
    emit threadCountChanged();
    emit contextLengthChanged();
    emit batchSizeChanged();
    emit maxTokensChanged();
    emit topKChanged();
    emit batchThreadsChanged();
    emit temperatureChanged();
    emit topPChanged();
    emit repeatPenaltyChanged();
    emit configChanged();
}

void LLMConfigManager::loadFromSettings(){
    LLMConfig defaultConfig;
    QSettings settings;
    settings.beginGroup("LLM");
    mSystemPrompt           = settings.value("systemPrompt", mSystemPrompt).toString();
    mConfig.threadCount     = settings.value("threadCount", defaultConfig.threadCount).toInt();
    mConfig.contextLength   = settings.value("contextLength", defaultConfig.contextLength).toInt();
    mConfig.batchSize       = settings.value("batchSize", defaultConfig.batchSize).toInt();
    mConfig.maxTokens       = settings.value("maxTokens", defaultConfig.maxTokens).toInt();
    mConfig.topK            = settings.value("topK", defaultConfig.topK).toInt();
    mConfig.batchThreads    = settings.value("batchThreads", defaultConfig.batchThreads).toInt();
    mConfig.temperature     = settings.value("temperature", defaultConfig.temperature).toFloat();
    mConfig.topP            = settings.value("topP", defaultConfig.topP).toFloat();
    mConfig.repeatPenalty   = settings.value("repeatPenalty", defaultConfig.repeatPenalty).toFloat();
    mConfig.modelPath       = settings.value("modelPath", defaultConfig.modelPath).toString();
    settings.endGroup();
}

void LLMConfigManager::saveToSettings(){
    QSettings settings;
    settings.beginGroup("LLM");
    settings.setValue("systemPrompt", mSystemPrompt);
    settings.setValue("threadCount", mConfig.threadCount);
    settings.setValue("contextLength", mConfig.contextLength);
    settings.setValue("batchSize", mConfig.batchSize);
    settings.setValue("maxTokens", mConfig.maxTokens);
    settings.setValue("topK", mConfig.topK);
    settings.setValue("batchThreads", mConfig.batchThreads);
    settings.setValue("temperature", mConfig.temperature);
    settings.setValue("topP", mConfig.topP);
    settings.setValue("repeatPenalty", mConfig.repeatPenalty);
    settings.setValue("modelPath", mConfig.modelPath);

    settings.endGroup();
    settings.sync();
}

