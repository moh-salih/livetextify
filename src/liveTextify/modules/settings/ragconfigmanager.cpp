#include "liveTextify/modules/settings/ragconfigmanager.h"
#include "liveTextify/core/Logger.h"
#include <QSettings>

RAGConfigManager::RAGConfigManager(QObject * parent): QObject(parent) {}

void RAGConfigManager::setEnabled(bool value){
    if(mEnabled == value) return;
    mEnabled = value;

    emit enabledChanged();
}

void RAGConfigManager::setMinChunkSize(int value){
    if(mConfig.chunking.minChunkLength == value) return;
    mConfig.chunking.minChunkLength = value;

    emit minChunkSizeChanged();
}

void RAGConfigManager::setMaxChunkSize(int value){
    if(mConfig.chunking.maxChunkLength == value) return;
    mConfig.chunking.maxChunkLength = value;

    emit maxChunkSizeChanged();
}

void RAGConfigManager::setTopKResults(int value){
    if(mConfig.search.topK == value) return;
    mConfig.search.topK = value;

    emit topKResultsChanged();
}

void RAGConfigManager::setEmbeddingDimension(int value){
    if(mConfig.index.dim == value) return;
    mConfig.index.dim = value;

    emit embeddingDimensionChanged();
}

void RAGConfigManager::setMinSimilarityScore(qreal value){
    if(qFuzzyCompare(mConfig.search.minSimilarity, static_cast<float>(value))) return;
    mConfig.search.minSimilarity = value;

    emit minSimilarityScoreChanged();
}

void RAGConfigManager::setLengthBoostFactor(qreal value){
    if(qFuzzyCompare(mConfig.chunking.lengthBoostFactor, static_cast<float>(value))) return;
    mConfig.chunking.lengthBoostFactor = value;

    emit lengthBoostFactorChanged();
}

void RAGConfigManager::setFillerThreshold(qreal value){
    if(qFuzzyCompare(mConfig.chunking.fillerWordLimit, static_cast<float>(value))) return;
    mConfig.chunking.fillerWordLimit = value;

    emit fillerThresholdChanged();
}

void RAGConfigManager::resetToDefaults(){
    mConfig = RagConfig();
    mEnabled = true;

    emit enabledChanged();
    emit minChunkSizeChanged();
    emit maxChunkSizeChanged();
    emit topKResultsChanged();
    emit embeddingDimensionChanged();
    emit minSimilarityScoreChanged();
    emit lengthBoostFactorChanged();
    emit fillerThresholdChanged();
    emit configChanged();
}

void RAGConfigManager::loadFromSettings() {
    RagConfig def;
    QSettings s;
    s.beginGroup("RAG");
    mEnabled                           = s.value("enabled", true).toBool();
    mConfig.chunking.minChunkLength    = s.value("minChars",     def.chunking.minChunkLength).toInt();
    mConfig.chunking.maxChunkLength    = s.value("maxChars",     def.chunking.maxChunkLength).toInt();
    mConfig.chunking.lengthBoostFactor = s.value("lengthBoost",  def.chunking.lengthBoostFactor).toFloat();
    mConfig.chunking.fillerWordLimit   = s.value("fillerLimit",  def.chunking.fillerWordLimit).toFloat();
    mConfig.index.dim                  = s.value("dim",          def.index.dim).toInt();
    mConfig.search.topK                = s.value("topK",         def.search.topK).toInt();
    mConfig.search.minSimilarity       = s.value("minScore",     def.search.minSimilarity).toFloat();
    s.endGroup();
}

void RAGConfigManager::saveToSettings() {
    QSettings s;
    s.beginGroup("RAG");
    s.setValue("enabled",     mEnabled);
    s.setValue("minChars",    mConfig.chunking.minChunkLength);
    s.setValue("maxChars",    mConfig.chunking.maxChunkLength);
    s.setValue("lengthBoost", mConfig.chunking.lengthBoostFactor);
    s.setValue("fillerLimit", mConfig.chunking.fillerWordLimit);
    s.setValue("dim",         mConfig.index.dim);
    s.setValue("topK",        mConfig.search.topK);
    s.setValue("minScore",    mConfig.search.minSimilarity);
    s.endGroup();
    s.sync();
}

