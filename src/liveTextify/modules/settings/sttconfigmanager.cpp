#include "liveTextify/modules/settings/sttconfigmanager.h"
#include "liveTextify/core/Logger.h"
#include <QSettings>

STTConfigManager::STTConfigManager(QObject * parent): QObject(parent){
    loadFromSettings();
}

void STTConfigManager::setMaxTokens(int value){
    if(mConfig.maxTokens == value) return;
    mConfig.maxTokens = value;
    saveToSettings();
    emit maxTokensChanged();
}

void STTConfigManager::setUseGpu(bool value){
    if(mConfig.useGpu == value) return;
    mConfig.useGpu = value;
    saveToSettings();
    emit useGpuChanged();
}

void STTConfigManager::setLanguage(const QString& value){
    if(mConfig.language == value) return;
    mConfig.language = value;
    saveToSettings();
    emit languageChanged();
}

void STTConfigManager::setThreadCount(int value){
    if(mConfig.threadCount == value) return;
    mConfig.threadCount = value;
    saveToSettings();
    emit threadCountChanged();
}

void STTConfigManager::setTemperature(qreal value){
    if(qFuzzyCompare(mConfig.temperature, static_cast<float>(value))) return;
    mConfig.temperature = value;
    saveToSettings();
    emit temperatureChanged();
}

void STTConfigManager::setTranslate(bool value){
    if(mConfig.translate == value) return;
    mConfig.translate = value;
    saveToSettings();
    emit translateChanged();
}

void STTConfigManager::setAutoDetectLanguage(bool value){
    if(mConfig.autoDetectLanguage == value) return;
    mConfig.autoDetectLanguage = value;
    saveToSettings();
    emit autoDetectLanguageChanged();
}

void STTConfigManager::resetToDefaults(){
    mConfig = STTConfig();
    saveToSettings();

    emit maxTokensChanged();
    emit useGpuChanged();
    emit languageChanged();
    emit threadCountChanged();
    emit temperatureChanged();
    emit translateChanged();
    emit autoDetectLanguageChanged();
}

void STTConfigManager::loadFromSettings(){
    QSettings settings;
    STTConfig defaultConfig;

    settings.beginGroup("STT");
    mConfig.maxTokens          = settings.value("maxTokens", defaultConfig.maxTokens).toInt();
    mConfig.language           = settings.value("language", defaultConfig.language).toString();
    mConfig.temperature        = settings.value("temperature", defaultConfig.temperature).toFloat();
    mConfig.threadCount        = settings.value("threadCount", defaultConfig.threadCount).toInt();
    mConfig.useGpu             = settings.value("useGpu", defaultConfig.useGpu).toBool();
    mConfig.translate          = settings.value("translate", defaultConfig.translate).toBool();
    mConfig.autoDetectLanguage = settings.value("autoDetectLanguage", defaultConfig.autoDetectLanguage).toBool();
    settings.endGroup();

    Logger::info("=== STT CONFIG LOADED ===");
    Logger::info(QString("  Model Path: %1").arg(mConfig.modelPath.isEmpty() ? "(not set)" : mConfig.modelPath));
    Logger::info(QString("  Use GPU: %1").arg(mConfig.useGpu ? "true" : "false"));
    Logger::info(QString("  Language: %1").arg(mConfig.language));
    Logger::info(QString("  Auto Detect: %1").arg(mConfig.autoDetectLanguage ? "true" : "false"));
    Logger::info(QString("  Translate: %1").arg(mConfig.translate ? "true" : "false"));
    Logger::info(QString("  Max Tokens: %1").arg(mConfig.maxTokens));
    Logger::info(QString("  Thread Count: %1").arg(mConfig.threadCount));
    Logger::info(QString("  Temperature: %1").arg(mConfig.temperature));
}

void STTConfigManager::saveToSettings(){
    QSettings settings;
    settings.beginGroup("STT");
    settings.setValue("maxTokens", mConfig.maxTokens);
    settings.setValue("temperature", mConfig.temperature);
    settings.setValue("useGpu", mConfig.useGpu);
    settings.setValue("language", mConfig.language);
    settings.setValue("threadCount", mConfig.threadCount);
    settings.setValue("translate", mConfig.translate);
    settings.setValue("autoDetectLanguage", mConfig.autoDetectLanguage);
    settings.endGroup();
    settings.sync();
}
