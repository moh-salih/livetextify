#include "whispersttengine.h"
#include "liveTextify/core/Logger.h"
#include <QDebug>
#include <QThread>



WhisperSttEngine::WhisperSttEngine(QObject *parent) : ISttEngine(parent) {}

WhisperSttEngine::~WhisperSttEngine() {
    if (m_ctx) whisper_free(m_ctx);
}

void WhisperSttEngine::setConfig(QSharedPointer<STTConfig> config){
    mSTTConfig = config;
}

bool WhisperSttEngine::abort_callback(void *user_data) {
    auto *engine = static_cast<WhisperSttEngine*>(user_data);
    return engine ? engine->m_abortRequested.load() : false;
}

void WhisperSttEngine::loadModel() {
    unloadModel();
    emit modelStatusChanged(LiveTextify::EngineStatus::Loading);

    if (!mSTTConfig || mSTTConfig->modelPath.isEmpty()) {
        Logger::error("WhisperSTT: Model path is empty in config.");
        emit errorEncountered(tr("STT model path is not configured."));
        emit modelStatusChanged(LiveTextify::EngineStatus::Error);
        return;
    }


    Logger::info("=== WHISPER STT ENGINE LOADING ===");
    Logger::info(QString("  Model Path: %1").arg(mSTTConfig->modelPath));
    Logger::info(QString("  Use GPU: %1").arg(mSTTConfig->useGpu ? "true" : "false"));
    Logger::info(QString("  Language: %1").arg(mSTTConfig->language));
    Logger::info(QString("  Auto Detect Language: %1").arg(mSTTConfig->autoDetectLanguage ? "true" : "false"));
    Logger::info(QString("  Translate: %1").arg(mSTTConfig->translate ? "true" : "false"));
    Logger::info(QString("  Max Tokens: %1").arg(mSTTConfig->maxTokens));
    Logger::info(QString("  Thread Count: %1").arg(mSTTConfig->threadCount));
    Logger::info(QString("  Temperature: %1").arg(mSTTConfig->temperature));

    // whisper_log_set([](enum ggml_log_level, const char*, void*){}, NULL);

    whisper_context_params params = whisper_context_default_params();
    params.use_gpu = mSTTConfig->useGpu;

    m_ctx = whisper_init_from_file_with_params(mSTTConfig->modelPath.toStdString().c_str(), params);

    if (!m_ctx) {
        emit errorEncountered("Failed to load Whisper model file");
        emit modelStatusChanged(LiveTextify::EngineStatus::Error);
        return;
    }
    emit modelStatusChanged(LiveTextify::EngineStatus::Ready);
}

void WhisperSttEngine::processWindow(const std::vector<float> &samples) {
    if (!m_ctx || samples.empty()) return;

    m_abortRequested.store(false);
    emit processingBusyChanged(true);

    whisper_full_params p = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);

    std::string langStd             = mSTTConfig->language.toStdString();

    p.detect_language               = mSTTConfig->autoDetectLanguage;
    p.language                      = langStd.c_str();
    p.translate                     = mSTTConfig->translate;
    p.max_tokens                    = mSTTConfig->maxTokens;
    p.n_threads                     = mSTTConfig->threadCount;
    p.no_context                    = true;
    p.single_segment                = true;
    p.print_progress                = false;
    p.temperature                   = mSTTConfig->temperature;
    p.abort_callback                = abort_callback;
    p.abort_callback_user_data      = this;
    p.suppress_blank                = true;




    int rc = whisper_full(m_ctx, p, samples.data(), static_cast<int>(samples.size()));

    if (!m_abortRequested.load() && rc == 0) {
        QString text;
        int n = whisper_full_n_segments(m_ctx);
        for (int i = 0; i < n; ++i) {
            text += QString::fromUtf8(whisper_full_get_segment_text(m_ctx, i));
        }
        if (!text.trimmed().isEmpty()) emit transcriptionSegmentReady(text.trimmed());
    } else if (rc != 0) {
        emit errorEncountered("Whisper inference failed");
    }

    emit processingBusyChanged(false);
}

void WhisperSttEngine::unloadModel() {
    m_abortRequested.store(true);
    if (m_ctx) { whisper_free(m_ctx); m_ctx = nullptr; }
    emit modelStatusChanged(LiveTextify::EngineStatus::Idle);
}

void WhisperSttEngine::stop() { m_abortRequested.store(true); }
void WhisperSttEngine::reset() { m_abortRequested.store(false); }
