#include "liveTextify/modules/settings/llmconfigmanager.h"
#include "liveTextify/core/Logger.h"
#include <QSettings>

LLMConfigManager::LLMConfigManager(QObject * parent): QObject(parent){

}

void LLMConfigManager::setSystemPrompt(const QString & value){
    if(mConfig.systemPrompt == value) return;
    mConfig.systemPrompt = value;
    saveToSettings();
    emit systemPromptChanged();
}

void LLMConfigManager::setThreadCount(int value){
    if(mConfig.threadCount == value) return;
    mConfig.threadCount = value;
    saveToSettings();
    emit threadCountChanged();
}

void LLMConfigManager::setContextLength(int value){
    if(mConfig.contextLength == value) return;
    mConfig.contextLength = value;
    saveToSettings();
    emit contextLengthChanged();
}

void LLMConfigManager::setBatchSize(int value){
    if(mConfig.batchSize == value) return;
    mConfig.batchSize = value;
    saveToSettings();
    emit batchSizeChanged();
}

void LLMConfigManager::setMaxTokens(int value){
    if(mConfig.maxTokens == value) return;
    mConfig.maxTokens = value;
    saveToSettings();
    emit maxTokensChanged();
}

void LLMConfigManager::setTopK(int value){
    if(mConfig.topK == value) return;
    mConfig.topK = value;
    saveToSettings();
    emit topKChanged();
}

void LLMConfigManager::setBatchThreads(int value){
    if(mConfig.batchThreads == value) return;
    mConfig.batchThreads = value;
    saveToSettings();
    emit batchThreadsChanged();
}

void LLMConfigManager::setTemperature(qreal value){
    if(qFuzzyCompare(mConfig.temperature, static_cast<float>(value))) return;
    mConfig.temperature = value;
    saveToSettings();
    emit temperatureChanged();
}

void LLMConfigManager::setTopP(qreal value){
    if(qFuzzyCompare(mConfig.topP, static_cast<float>(value))) return;
    mConfig.topP = value;
    saveToSettings();
    emit topPChanged();
}

void LLMConfigManager::setRepeatPenalty(qreal value){
    if(qFuzzyCompare(mConfig.repeatPenalty, static_cast<float>(value))) return;
    mConfig.repeatPenalty = value;
    saveToSettings();
    emit repeatPenaltyChanged();
}

void LLMConfigManager::resetToDefaults(){
    mConfig = LLMConfig();

    saveToSettings();

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
}

void LLMConfigManager::loadFromSettings(){
    LLMConfig defaultConfig;
    QSettings settings;
    settings.beginGroup("LLM");
    mConfig.systemPrompt    = settings.value("systemPrompt", defaultConfig.systemPrompt).toString();
    mConfig.threadCount     = settings.value("threadCount", defaultConfig.threadCount).toInt();
    mConfig.contextLength   = settings.value("contextLength", defaultConfig.contextLength).toInt();
    mConfig.batchSize       = settings.value("batchSize", defaultConfig.batchSize).toInt();
    mConfig.maxTokens       = settings.value("maxTokens", defaultConfig.maxTokens).toInt();
    mConfig.topK            = settings.value("topK", defaultConfig.topK).toInt();
    mConfig.batchThreads    = settings.value("batchThreads", defaultConfig.batchThreads).toInt();
    mConfig.temperature     = settings.value("temperature", defaultConfig.temperature).toFloat();
    mConfig.topP            = settings.value("topP", defaultConfig.topP).toFloat();
    mConfig.repeatPenalty   = settings.value("repeatPenalty", defaultConfig.repeatPenalty).toFloat();
    settings.endGroup();

    Logger::info("=== LLM CONFIG LOADED ===");
    Logger::info(QString("  Model Path: %1").arg(mConfig.modelPath.isEmpty() ? "(not set)" : mConfig.modelPath));
    Logger::info(QString("  System Prompt: %1").arg(mConfig.systemPrompt.left(50) + (mConfig.systemPrompt.length() > 50 ? "..." : "")));
    Logger::info(QString("  Thread Count: %1").arg(mConfig.threadCount));
    Logger::info(QString("  Batch Threads: %1").arg(mConfig.batchThreads));
    Logger::info(QString("  Context Length: %1").arg(mConfig.contextLength));
    Logger::info(QString("  Batch Size: %1").arg(mConfig.batchSize));
    Logger::info(QString("  Temperature: %1").arg(mConfig.temperature));
    Logger::info(QString("  Top P: %1").arg(mConfig.topP));
    Logger::info(QString("  Top K: %1").arg(mConfig.topK));
    Logger::info(QString("  Repeat Penalty: %1").arg(mConfig.repeatPenalty));
    Logger::info(QString("  Max Tokens: %1").arg(mConfig.maxTokens));
}

void LLMConfigManager::saveToSettings(){
    QSettings settings;
    settings.beginGroup("LLM");
    settings.setValue("systemPrompt", mConfig.systemPrompt);
    settings.setValue("threadCount", mConfig.threadCount);
    settings.setValue("contextLength", mConfig.contextLength);
    settings.setValue("batchSize", mConfig.batchSize);
    settings.setValue("maxTokens", mConfig.maxTokens);
    settings.setValue("topK", mConfig.topK);
    settings.setValue("batchThreads", mConfig.batchThreads);
    settings.setValue("temperature", mConfig.temperature);
    settings.setValue("topP", mConfig.topP);
    settings.setValue("repeatPenalty", mConfig.repeatPenalty);
    settings.endGroup();
    settings.sync();
}

