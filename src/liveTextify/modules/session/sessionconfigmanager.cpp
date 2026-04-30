// sessionconfigmanager.cpp
#include "sessionconfigmanager.h"

SessionConfigManager::SessionConfigManager(QObject* parent) : QObject(parent) {}

void SessionConfigManager::seedFrom(const SessionConfig& cfg) {
    mConfig = cfg;
    emit sessionSwitched(); // QML listens to this to refresh all bindings
}

void SessionConfigManager::clear() {
    mConfig = SessionConfig{};
    emit sessionSwitched();
}

// ── STT ───────────────────────────────────────────────────────────────────────

void SessionConfigManager::setSttModelPath(const QString& v) {
    if (mConfig.stt.modelPath == v) return;
    mConfig.stt.modelPath = v;
    emit sttModelPathChanged();
    emit configChanged();
}

void SessionConfigManager::setLanguage(const QString& v) {
    if (mConfig.stt.language == v) return;
    mConfig.stt.language = v;
    emit languageChanged();
    emit configChanged();
}

void SessionConfigManager::setAutoDetectLanguage(bool v) {
    if (mConfig.stt.autoDetectLanguage == v) return;
    mConfig.stt.autoDetectLanguage = v;
    emit autoDetectLanguageChanged();
    emit configChanged();
}

void SessionConfigManager::setTranslate(bool v) {
    if (mConfig.stt.translate == v) return;
    mConfig.stt.translate = v;
    emit translateChanged();
    emit configChanged();
}

// ── LLM ───────────────────────────────────────────────────────────────────────

void SessionConfigManager::setLlmModelPath(const QString& v) {
    if (mConfig.llm.modelPath == v) return;
    mConfig.llm.modelPath = v;
    emit llmModelPathChanged();
    emit configChanged();
}

void SessionConfigManager::setSystemPrompt(const QString& v) {
    if (mConfig.systemPrompt == v) return;
    mConfig.systemPrompt = v;
    emit systemPromptChanged();
    emit configChanged();
}

void SessionConfigManager::setTemperature(float v) {
    if (qFuzzyCompare(mConfig.llm.temperature, v)) return;
    mConfig.llm.temperature = v;
    emit temperatureChanged();
    emit configChanged();
}

void SessionConfigManager::setTopP(float v) {
    if (qFuzzyCompare(mConfig.llm.topP, v)) return;
    mConfig.llm.topP = v;
    emit topPChanged();
    emit configChanged();
}

void SessionConfigManager::setTopK(int v) {
    if (mConfig.llm.topK == v) return;
    mConfig.llm.topK = v;
    emit topKChanged();
    emit configChanged();
}

void SessionConfigManager::setRepeatPenalty(float v) {
    if (qFuzzyCompare(mConfig.llm.repeatPenalty, v)) return;
    mConfig.llm.repeatPenalty = v;
    emit repeatPenaltyChanged();
    emit configChanged();
}

void SessionConfigManager::setMaxTokens(int v) {
    if (mConfig.llm.maxTokens == v) return;
    mConfig.llm.maxTokens = v;
    emit maxTokensChanged();
    emit configChanged();
}

void SessionConfigManager::setContextLength(int v) {
    if (mConfig.llm.contextLength == v) return;
    mConfig.llm.contextLength = v;
    emit contextLengthChanged();
    emit configChanged();
}

void SessionConfigManager::setThreadCount(int v) {
    if (mConfig.llm.threadCount == v) return;
    mConfig.llm.threadCount = v;
    emit threadCountChanged();
    emit configChanged();
}

void SessionConfigManager::setBatchSize(int v) {
    if (mConfig.llm.batchSize == v) return;
    mConfig.llm.batchSize = v;
    emit batchSizeChanged();
    emit configChanged();
}

// ── Embedder ──────────────────────────────────────────────────────────────────

void SessionConfigManager::setEmbModelPath(const QString& v) {
    if (mConfig.emb.modelPath == v) return;
    mConfig.emb.modelPath = v;
    emit embModelPathChanged();
    emit configChanged();
}

// ── RAG ───────────────────────────────────────────────────────────────────────

void SessionConfigManager::setEnableRag(bool v) {
    if (mConfig.enableRag == v) return;
    mConfig.enableRag = v;
    emit enableRagChanged();
    emit configChanged();
}

void SessionConfigManager::setRagTopK(int v) {
    if (mConfig.rag.search.topK == v) return;
    mConfig.rag.search.topK = v;
    emit ragTopKChanged();
    emit configChanged();
}

void SessionConfigManager::setRagMinScore(float v) {
    if (qFuzzyCompare(mConfig.rag.search.minSimilarity, v)) return;
    mConfig.rag.search.minSimilarity = v;
    emit ragMinScoreChanged();
    emit configChanged();
}

void SessionConfigManager::setMinChunkSize(int v) {
    if (mConfig.rag.chunking.minChunkLength == v) return;
    mConfig.rag.chunking.minChunkLength = v;
    emit minChunkSizeChanged();
    emit configChanged();
}

void SessionConfigManager::setMaxChunkSize(int v) {
    if (mConfig.rag.chunking.maxChunkLength == v) return;
    mConfig.rag.chunking.maxChunkLength = v;
    emit maxChunkSizeChanged();
    emit configChanged();
}
