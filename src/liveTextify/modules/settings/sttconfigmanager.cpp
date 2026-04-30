#include "liveTextify/modules/settings/sttconfigmanager.h"
#include "liveTextify/core/Logger.h"
#include <QSettings>

STTConfigManager::STTConfigManager(QObject * parent): QObject(parent){

}

void STTConfigManager::setMaxTokens(int value){
    if(mConfig.maxTokens == value) return;
    mConfig.maxTokens = value;

    emit maxTokensChanged();
}

void STTConfigManager::setUseGpu(bool value){
    if(mConfig.useGpu == value) return;
    mConfig.useGpu = value;

    emit useGpuChanged();
}

void STTConfigManager::setLanguage(const QString& value){
    if(mConfig.language == value) return;
    mConfig.language = value;

    emit languageChanged();
}

void STTConfigManager::setThreadCount(int value){
    if(mConfig.threadCount == value) return;
    mConfig.threadCount = value;

    emit threadCountChanged();
}

void STTConfigManager::setTemperature(qreal value){
    if(qFuzzyCompare(mConfig.temperature, static_cast<float>(value))) return;
    mConfig.temperature = value;

    emit temperatureChanged();
}

void STTConfigManager::setTranslate(bool value){
    if(mConfig.translate == value) return;
    mConfig.translate = value;

    emit translateChanged();
}

void STTConfigManager::setAutoDetectLanguage(bool value){
    if(mConfig.autoDetectLanguage == value) return;
    mConfig.autoDetectLanguage = value;

    emit autoDetectLanguageChanged();
}

void STTConfigManager::setModelPath(const QString& v) {
    if (mConfig.modelPath == v) return;
    mConfig.modelPath = v;
    emit configChanged();
}
void STTConfigManager::resetToDefaults(){
    mConfig = SttConfig();

    emit maxTokensChanged();
    emit useGpuChanged();
    emit languageChanged();
    emit threadCountChanged();
    emit temperatureChanged();
    emit translateChanged();
    emit autoDetectLanguageChanged();
    emit configChanged();
}

void STTConfigManager::loadFromSettings(){
    QSettings settings;
    SttConfig defaultConfig;

    settings.beginGroup("STT");
    mConfig.maxTokens          = settings.value("maxTokens", defaultConfig.maxTokens).toInt();
    mConfig.language           = settings.value("language", defaultConfig.language).toString();
    mConfig.temperature        = settings.value("temperature", defaultConfig.temperature).toFloat();
    mConfig.threadCount        = settings.value("threadCount", defaultConfig.threadCount).toInt();
    mConfig.useGpu             = settings.value("useGpu", defaultConfig.useGpu).toBool();
    mConfig.translate          = settings.value("translate", defaultConfig.translate).toBool();
    mConfig.autoDetectLanguage = settings.value("autoDetectLanguage", defaultConfig.autoDetectLanguage).toBool();
    mConfig.modelPath          = settings.value("modelPath", defaultConfig.modelPath).toString();
    settings.endGroup();
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
    settings.setValue("modelPath", mConfig.modelPath);
    settings.endGroup();
    settings.sync();
}
