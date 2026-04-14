#include "liveTextify/modules/settings/embconfigmanager.h"
#include "liveTextify/core/Logger.h"
#include <QSettings>

EmbConfigManager::EmbConfigManager(QObject * parent) {}

void EmbConfigManager::setThreadCount(int value){
    if(mConfig.nThreads == value) return;
    mConfig.nThreads = value;
    saveToSettings();
    emit threadCountChanged();
}

void EmbConfigManager::setContextLength(int value){
    if(mConfig.nCtx == value) return;
    mConfig.nCtx = value;
    saveToSettings();
    emit contextLengthChanged();
}

void EmbConfigManager::resetToDefaults(){
    mConfig = EmbConfig();
    saveToSettings();
    emit threadCountChanged();
}

void EmbConfigManager::loadFromSettings(){
    EmbConfig defaultConfig;
    QSettings settings;

    settings.beginGroup("EMB");
    mConfig.nThreads = settings.value("nThreads", defaultConfig.nThreads).toInt();
    mConfig.nCtx = settings.value("nCtx", defaultConfig.nCtx).toInt();
    settings.endGroup();

    Logger::info("=== EMBEDDING CONFIG LOADED ===");
    Logger::info(QString("  Model Path: %1").arg(mConfig.modelPath.isEmpty() ? "(not set)" : mConfig.modelPath));
    Logger::info(QString("  Threads: %1").arg(mConfig.nThreads));
    Logger::info(QString("  Context Size (nCtx): %1").arg(mConfig.nCtx));
    Logger::info(QString("  GPU Layers: %1").arg(mConfig.nGpuLayers));
}

void EmbConfigManager::saveToSettings(){
    QSettings settings;
    settings.beginGroup("EMB");
    settings.setValue("nThreads", mConfig.nThreads);
    settings.setValue("nCtx", mConfig.nCtx);
    settings.endGroup();
    settings.sync();
}
