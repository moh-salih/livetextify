#include "liveTextify/modules/settings/ragconfigmanager.h"
#include "liveTextify/core/Logger.h"
#include <QSettings>

RAGConfigManager::RAGConfigManager(QObject * parent): QObject(parent) {}

void RAGConfigManager::setEnabled(bool value){
    if(mConfig.enabled == value) return;
    mConfig.enabled = value;
    saveToSettings();
    emit enabledChanged();
}

void RAGConfigManager::setMinChunkSize(int value){
    if(mConfig.minChars == value) return;
    mConfig.minChars = value;
    saveToSettings();
    emit minChunkSizeChanged();
}

void RAGConfigManager::setMaxChunkSize(int value){
    if(mConfig.maxChars == value) return;
    mConfig.maxChars = value;
    saveToSettings();
    emit maxChunkSizeChanged();
}

void RAGConfigManager::setTopKResults(int value){
    if(mConfig.topK == value) return;
    mConfig.topK = value;
    saveToSettings();
    emit topKResultsChanged();
}

void RAGConfigManager::setEmbeddingDimension(int value){
    if(mConfig.dim == value) return;
    mConfig.dim = value;
    saveToSettings();
    emit embeddingDimensionChanged();
}

void RAGConfigManager::setMinSimilarityScore(qreal value){
    if(qFuzzyCompare(mConfig.minScore, static_cast<float>(value))) return;
    mConfig.minScore = value;
    saveToSettings();
    emit minSimilarityScoreChanged();
}

void RAGConfigManager::setLengthBoostFactor(qreal value){
    if(qFuzzyCompare(mConfig.lengthBoost, static_cast<float>(value))) return;
    mConfig.lengthBoost = value;
    saveToSettings();
    emit lengthBoostFactorChanged();
}

void RAGConfigManager::setFillerThreshold(qreal value){
    if(qFuzzyCompare(mConfig.fillerLimit, static_cast<float>(value))) return;
    mConfig.fillerLimit = value;
    saveToSettings();
    emit fillerThresholdChanged();
}

void RAGConfigManager::resetToDefaults(){
    mConfig = RAGConfig();
    saveToSettings();

    emit enabledChanged();
    emit minChunkSizeChanged();
    emit maxChunkSizeChanged();
    emit topKResultsChanged();
    emit embeddingDimensionChanged();
    emit minSimilarityScoreChanged();
    emit lengthBoostFactorChanged();
    emit fillerThresholdChanged();
}

void RAGConfigManager::loadFromSettings(){
    RAGConfig defaultConfig;
    QSettings settings;

    settings.beginGroup("RAG");
    mConfig.enabled         = settings.value("enabled", defaultConfig.enabled).toBool();
    mConfig.minChars        = settings.value("minChars", defaultConfig.minChars).toInt();
    mConfig.maxChars        = settings.value("maxChars", defaultConfig.maxChars).toInt();
    mConfig.topK            = settings.value("topK", defaultConfig.topK).toInt();
    mConfig.dim             = settings.value("dim", defaultConfig.dim).toInt();
    mConfig.minScore        = settings.value("minScore", defaultConfig.minScore).toFloat();
    mConfig.lengthBoost     = settings.value("lengthBoost", defaultConfig.lengthBoost).toFloat();
    mConfig.fillerLimit     = settings.value("fillerLimit", defaultConfig.fillerLimit).toFloat();
    settings.endGroup();

    Logger::info("=== RAG CONFIG LOADED ===");
    Logger::info(QString("  Enabled: %1").arg(mConfig.enabled ? "true" : "false"));
    Logger::info(QString("  Min Chars: %1, Max Chars: %2").arg(mConfig.minChars).arg(mConfig.maxChars));
    Logger::info(QString("  Top K: %1, Dimension: %2").arg(mConfig.topK).arg(mConfig.dim));
    Logger::info(QString("  Min Score: %1, Length Boost: %2, Filler Limit: %3").arg(mConfig.minScore).arg(mConfig.lengthBoost).arg(mConfig.fillerLimit));
}

void RAGConfigManager::saveToSettings(){
    QSettings settings;
    settings.beginGroup("RAG");
    settings.setValue("enabled", mConfig.enabled);
    settings.setValue("minChars", mConfig.minChars);
    settings.setValue("maxChars", mConfig.maxChars);
    settings.setValue("topK", mConfig.topK);
    settings.setValue("dim", mConfig.dim);
    settings.setValue("minScore", mConfig.minScore);
    settings.setValue("lengthBoost", mConfig.lengthBoost);
    settings.setValue("fillerLimit", mConfig.fillerLimit);
    settings.endGroup();
    settings.sync();
}

