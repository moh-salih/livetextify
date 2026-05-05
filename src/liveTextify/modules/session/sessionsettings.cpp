#include "liveTextify/modules/session/sessionsettings.h"
#include <QSettings>

SessionSettings::SessionSettings(QObject* parent): QObject(parent){

}

// ── Lifecycle ─────────────────────────────────────────────────────────────────

void SessionSettings::load() {
    QSettings s;

    // Only load what is user-facing (has a Q_PROPERTY).
    // All other fields on mConfig stay at their library default-constructed values.

    s.beginGroup("STT");
    mConfig.stt.language           = s.value("language",           mConfig.stt.language).toString();
    mConfig.stt.autoDetectLanguage = s.value("autoDetectLanguage", mConfig.stt.autoDetectLanguage).toBool();
    mConfig.stt.translate          = s.value("translate",          mConfig.stt.translate).toBool();
    mConfig.stt.modelPath          = s.value("modelPath",          mConfig.stt.modelPath).toString();
    s.endGroup();

    s.beginGroup("LLM");
    mConfig.systemPrompt      = s.value("systemPrompt",  mConfig.systemPrompt).toString();
    mConfig.llm.temperature   = s.value("temperature",   mConfig.llm.temperature).toFloat();
    mConfig.llm.topP          = s.value("topP",          mConfig.llm.topP).toFloat();
    mConfig.llm.topK          = s.value("topK",          mConfig.llm.topK).toInt();
    mConfig.llm.repeatPenalty = s.value("repeatPenalty", mConfig.llm.repeatPenalty).toFloat();
    mConfig.llm.maxTokens     = s.value("maxTokens",     mConfig.llm.maxTokens).toInt();
    mConfig.llm.modelPath     = s.value("modelPath",     mConfig.llm.modelPath).toString();
    mConfig.llm.contextLength = s.value("contextLength", mConfig.llm.contextLength).toInt();
    s.endGroup();

    s.beginGroup("EMB");
    mConfig.emb.modelPath = s.value("modelPath", mConfig.emb.modelPath).toString();
    s.endGroup();

    s.beginGroup("RAG");
    mConfig.enableRag                      = s.value("enabled",     mConfig.enableRag).toBool();
    mConfig.rag.chunking.minChunkLength    = s.value("minChars",    mConfig.rag.chunking.minChunkLength).toInt();
    mConfig.rag.chunking.maxChunkLength    = s.value("maxChars",    mConfig.rag.chunking.maxChunkLength).toInt();
    mConfig.rag.chunking.lengthBoostFactor = s.value("lengthBoost", mConfig.rag.chunking.lengthBoostFactor).toFloat();
    mConfig.rag.chunking.fillerWordLimit   = s.value("fillerLimit", mConfig.rag.chunking.fillerWordLimit).toFloat();
    mConfig.rag.index.dim                  = s.value("dim",         mConfig.rag.index.dim).toInt();
    mConfig.rag.search.topK                = s.value("topK",        mConfig.rag.search.topK).toInt();
    mConfig.rag.search.minSimilarity       = s.value("minScore",    mConfig.rag.search.minSimilarity).toFloat();
    s.endGroup();

    s.beginGroup("AUDIO");
    mConfig.audio.windowDurationMs = s.value("maxWindow", mConfig.audio.windowDurationMs).toInt();
    mConfig.audio.stepDurationMs   = s.value("step",      mConfig.audio.stepDurationMs).toInt();
    s.endGroup();

    emit configChanged(mConfig);
}

// ── Persistence ───────────────────────────────────────────────────────────────
// Only persist user-facing fields. Internal fields (threadCount, contextLength,
// etc.) are intentionally omitted — they belong to the libraries' own defaults.

void SessionSettings::save() {
    QSettings s;

    s.beginGroup("STT");
    s.setValue("language",           mConfig.stt.language);
    s.setValue("autoDetectLanguage", mConfig.stt.autoDetectLanguage);
    s.setValue("translate",          mConfig.stt.translate);
    s.setValue("modelPath",          mConfig.stt.modelPath);
    s.endGroup();

    s.beginGroup("LLM");
    s.setValue("systemPrompt",  mConfig.systemPrompt);
    s.setValue("temperature",   mConfig.llm.temperature);
    s.setValue("topP",          mConfig.llm.topP);
    s.setValue("topK",          mConfig.llm.topK);
    s.setValue("repeatPenalty", mConfig.llm.repeatPenalty);
    s.setValue("maxTokens",     mConfig.llm.maxTokens);
    s.setValue("modelPath",     mConfig.llm.modelPath);
    s.endGroup();

    s.beginGroup("EMB");
    s.setValue("modelPath", mConfig.emb.modelPath);
    s.endGroup();

    s.beginGroup("RAG");
    s.setValue("enabled",     mConfig.enableRag);
    s.setValue("minChars",    mConfig.rag.chunking.minChunkLength);
    s.setValue("maxChars",    mConfig.rag.chunking.maxChunkLength);
    s.setValue("lengthBoost", mConfig.rag.chunking.lengthBoostFactor);
    s.setValue("fillerLimit", mConfig.rag.chunking.fillerWordLimit);
    s.setValue("dim",         mConfig.rag.index.dim);
    s.setValue("topK",        mConfig.rag.search.topK);
    s.setValue("minScore",    mConfig.rag.search.minSimilarity);
    s.endGroup();

    s.beginGroup("AUDIO");
    s.setValue("maxWindow", mConfig.audio.windowDurationMs);
    s.setValue("step",      mConfig.audio.stepDurationMs);
    s.endGroup();

    s.sync();
}

void SessionSettings::notifyChange() {
    save();
    emit configChanged(mConfig);
}

// ── Bridge ────────────────────────────────────────────────────────────────────

void SessionSettings::seedFrom(const SessionConfig& cfg) {
    // Copy only user-visible fields — internal ones (threadCount, modelPath, etc.)
    // are owned by this class and must not be overwritten by per-session data.
    mConfig.stt.language           = cfg.stt.language;
    mConfig.stt.autoDetectLanguage = cfg.stt.autoDetectLanguage;
    mConfig.stt.translate          = cfg.stt.translate;

    mConfig.systemPrompt      = cfg.systemPrompt;
    mConfig.llm.temperature   = cfg.llm.temperature;
    mConfig.llm.topP          = cfg.llm.topP;
    mConfig.llm.topK          = cfg.llm.topK;
    mConfig.llm.repeatPenalty = cfg.llm.repeatPenalty;
    mConfig.llm.maxTokens     = cfg.llm.maxTokens;
    mConfig.llm.contextLength = cfg.llm.contextLength;

    mConfig.enableRag                      = cfg.enableRag;
    mConfig.rag.search.topK                = cfg.rag.search.topK;
    mConfig.rag.search.minSimilarity       = cfg.rag.search.minSimilarity;
    mConfig.rag.chunking.minChunkLength    = cfg.rag.chunking.minChunkLength;
    mConfig.rag.chunking.maxChunkLength    = cfg.rag.chunking.maxChunkLength;

    mConfig.audio.stepDurationMs   = cfg.audio.stepDurationMs;
    mConfig.audio.windowDurationMs = cfg.audio.windowDurationMs;

    emit configChanged(mConfig);
}

SessionConfig SessionSettings::toConfig() const {
    return mConfig;
}

// ── Model path ────────────────────────────────────────────────────────────────

void SessionSettings::setModelPath(const QString& type, const QString& path) {
    if      (type == "stt") mConfig.stt.modelPath = path;
    else if (type == "llm") mConfig.llm.modelPath = path;
    else if (type == "emb") mConfig.emb.modelPath = path;
    else return;

    notifyChange();
}

// ── STT setters ───────────────────────────────────────────────────────────────
void SessionSettings::setLanguage(const QString& v) {
    if (mConfig.stt.language == v) return;
    mConfig.stt.language = v;
    emit languageChanged();
    notifyChange();
}

void SessionSettings::setAutoDetectLanguage(bool v) {
    if (mConfig.stt.autoDetectLanguage == v) return;
    mConfig.stt.autoDetectLanguage = v;
    emit autoDetectLanguageChanged();
    notifyChange();
}

void SessionSettings::setTranslate(bool v) {
    if (mConfig.stt.translate == v) return;
    mConfig.stt.translate = v;
    emit translateChanged();
    notifyChange();
}

// ── LLM setters ───────────────────────────────────────────────────────────────

void SessionSettings::setSystemPrompt(const QString& v) {
    if (mConfig.systemPrompt == v) return;
    mConfig.systemPrompt = v;
    emit systemPromptChanged();
    notifyChange();
}

void SessionSettings::setTemperature(float v) {
    if (qFuzzyCompare(mConfig.llm.temperature, v)) return;
    mConfig.llm.temperature = v;
    emit temperatureChanged();
    notifyChange();
}

void SessionSettings::setTopP(float v) {
    if (qFuzzyCompare(mConfig.llm.topP, v)) return;
    mConfig.llm.topP = v;
    emit topPChanged();
    notifyChange();
}

void SessionSettings::setTopK(int v) {
    if (mConfig.llm.topK == v) return;
    mConfig.llm.topK = v;
    emit topKChanged();
    notifyChange();
}

void SessionSettings::setRepeatPenalty(float v) {
    if (qFuzzyCompare(mConfig.llm.repeatPenalty, v)) return;
    mConfig.llm.repeatPenalty = v;
    emit repeatPenaltyChanged();
    notifyChange();
}

void SessionSettings::setMaxTokens(int v) {
    if (mConfig.llm.maxTokens == v) return;
    mConfig.llm.maxTokens = v;
    emit maxTokensChanged();
    notifyChange();
}

void SessionSettings::onContextLengthResolved(int length) {
    if (mResolvedContextLength == length) return;
    mResolvedContextLength = length;
    emit resolvedContextLengthChanged();
}

void SessionSettings::setLlmContextLength(int v){
    if(mConfig.llm.contextLength == v) return;
    mConfig.llm.contextLength = v;
    emit llmContextLengthChanged();
    notifyChange();
}

// ── RAG setters ───────────────────────────────────────────────────────────────

void SessionSettings::setEnableRag(bool v) {
    if (mConfig.enableRag == v) return;
    mConfig.enableRag = v;
    emit enableRagChanged();
    notifyChange();
}

void SessionSettings::setRagTopK(int v) {
    if (mConfig.rag.search.topK == v) return;
    mConfig.rag.search.topK = v;
    emit ragTopKChanged();
    notifyChange();
}

void SessionSettings::setRagMinScore(float v) {
    if (qFuzzyCompare(mConfig.rag.search.minSimilarity, v)) return;
    mConfig.rag.search.minSimilarity = v;
    emit ragMinScoreChanged();
    notifyChange();
}

void SessionSettings::setMinChunkSize(int v) {
    if (mConfig.rag.chunking.minChunkLength == v) return;
    mConfig.rag.chunking.minChunkLength = v;
    emit minChunkSizeChanged();
    notifyChange();
}

void SessionSettings::setMaxChunkSize(int v) {
    if (mConfig.rag.chunking.maxChunkLength == v) return;
    mConfig.rag.chunking.maxChunkLength = v;
    emit maxChunkSizeChanged();
    notifyChange();
}

// ── Audio setters ─────────────────────────────────────────────────────────────

void SessionSettings::setStep(int v) {
    if (mConfig.audio.stepDurationMs == v) return;
    mConfig.audio.stepDurationMs = v;
    emit stepChanged();
    emit keepChanged();
    notifyChange();
}

void SessionSettings::setKeep(int v) {
    const int newStep = mConfig.audio.windowDurationMs - v;
    if (mConfig.audio.stepDurationMs == newStep) return;
    mConfig.audio.stepDurationMs = newStep;
    emit stepChanged();
    emit keepChanged();
    notifyChange();
}

void SessionSettings::setMaxWindow(int v) {
    if (mConfig.audio.windowDurationMs == v) return;
    mConfig.audio.windowDurationMs = v;
    emit maxWindowChanged();
    emit keepChanged();
    notifyChange();
}

// ── QML callables ─────────────────────────────────────────────────────────────
void SessionSettings::approveReload() {
    emit reloadApproved();
}

void SessionSettings::resetToDefaults() {
    QSettings settings;
    settings.remove("STT");
    settings.remove("LLM");
    settings.remove("EMB");
    settings.remove("RAG");
    settings.remove("AUDIO");
    settings.sync();

    mConfig.stt         = SttConfig();
    mConfig.llm         = LLMConfig();
    mConfig.emb         = EmbConfig();
    mConfig.rag         = RagConfig();
    mConfig.audio       = AudioConfig();
    mConfig.enableRag    = true;


    emit languageChanged();
    emit autoDetectLanguageChanged();
    emit translateChanged();

    emit systemPromptChanged();
    emit temperatureChanged();
    emit topPChanged();
    emit topKChanged();
    emit repeatPenaltyChanged();
    emit maxTokensChanged();
    emit llmContextLengthChanged();

    emit enableRagChanged();
    emit ragTopKChanged();
    emit ragMinScoreChanged();
    emit minChunkSizeChanged();
    emit maxChunkSizeChanged();

    emit stepChanged();
    emit keepChanged();
    emit maxWindowChanged();

    notifyChange();
}
