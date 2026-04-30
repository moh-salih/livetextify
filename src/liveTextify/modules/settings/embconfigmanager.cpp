#include "liveTextify/modules/settings/embconfigmanager.h"
#include "liveTextify/core/Logger.h"
#include <QSettings>


EmbConfigManager::EmbConfigManager(QObject * parent): QObject(parent) {}

void EmbConfigManager::setThreadCount(int value){
    if(mConfig.nThreads == value) return;
    mConfig.nThreads = value;
    emit threadCountChanged();
}

void EmbConfigManager::setContextLength(int value){
    if(mConfig.nCtx == value) return;
    mConfig.nCtx = value;
    emit contextLengthChanged();
}

void EmbConfigManager::resetToDefaults(){
    mConfig = EmbConfig();

    emit threadCountChanged();
    emit contextLengthChanged();
    emit configChanged();
}

void EmbConfigManager::setModelPath(const QString& v) {
    if (mConfig.modelPath == v) return;
    mConfig.modelPath = v;
    emit configChanged();
}

void EmbConfigManager::loadFromSettings(){
    EmbConfig defaultConfig;
    QSettings settings;

    settings.beginGroup("EMB");
    mConfig.nThreads = settings.value("nThreads", defaultConfig.nThreads).toInt();
    mConfig.nCtx = settings.value("nCtx", defaultConfig.nCtx).toInt();
    mConfig.modelPath = settings.value("modelPath", defaultConfig.modelPath).toString();
    settings.endGroup();
}

void EmbConfigManager::saveToSettings(){
    QSettings settings;
    settings.beginGroup("EMB");
    settings.setValue("nThreads", mConfig.nThreads);
    settings.setValue("nCtx", mConfig.nCtx);
    settings.setValue("modelPath", mConfig.modelPath);
    settings.endGroup();
    settings.sync();
}
