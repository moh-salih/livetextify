#include "llamallmengine.h"
#include <QThread>
#include <QCoreApplication>
#include <QDebug>

#include "liveTextify/core/Logger.h"

using namespace LiveTextify;


LlamaLlmEngine::LlamaLlmEngine(QObject *parent) : ILlmEngine(parent) {
    llama_backend_init();
    // llama_log_set([](enum ggml_log_level, const char*, void*){}, NULL);
}

void LlamaLlmEngine::loadModel() {
    emit modelStatusChanged(EngineStatus::Loading);

    if (!mLLMConfig || mLLMConfig->modelPath.isEmpty()) {
        Logger::error("LlamaLLM: Model path is empty in config.");
        emit errorEncountered("LLM model path is not configured.");
        emit modelStatusChanged(EngineStatus::Error);
        return;
    }


    Logger::info("=== LLAMA LLM ENGINE LOADING ===");
    Logger::info(QString("  Model Path: %1").arg(mLLMConfig->modelPath));
    Logger::info(QString("  System Prompt: %1").arg(mLLMConfig->systemPrompt.left(100) + (mLLMConfig->systemPrompt.length() > 100 ? "..." : "")));
    Logger::info(QString("  Thread Count: %1").arg(mLLMConfig->threadCount));
    Logger::info(QString("  Batch Threads: %1").arg(mLLMConfig->batchThreads));
    Logger::info(QString("  Context Length: %1").arg(mLLMConfig->contextLength));
    Logger::info(QString("  Batch Size: %1").arg(mLLMConfig->batchSize));
    Logger::info(QString("  Temperature: %1").arg(mLLMConfig->temperature));
    Logger::info(QString("  Top P: %1").arg(mLLMConfig->topP));
    Logger::info(QString("  Top K: %1").arg(mLLMConfig->topK));
    Logger::info(QString("  Repeat Penalty: %1").arg(mLLMConfig->repeatPenalty));
    Logger::info(QString("  Max Tokens: %1").arg(mLLMConfig->maxTokens == -1 ? "unlimited" : QString::number(mLLMConfig->maxTokens)));


    llama_model_params mp = llama_model_default_params();
    m_model = llama_model_load_from_file(mLLMConfig->modelPath.toStdString().c_str(), mp);

    if (!m_model) {
        Logger::error(QString("LlamaLLM: Failed to load model from %1").arg(mLLMConfig->modelPath));
        emit errorEncountered("Failed to load LLM model file");
        emit modelStatusChanged(EngineStatus::Error);
        return;
    }

    llama_context_params cp = llama_context_default_params();
    cp.n_threads        = mLLMConfig->threadCount;
    cp.n_ctx            = mLLMConfig->contextLength;
    cp.n_batch          = mLLMConfig->batchSize;
    cp.n_threads_batch  = mLLMConfig->batchThreads;

    m_ctx = llama_init_from_model(m_model, cp);

    if (!m_ctx) {
        emit modelStatusChanged(EngineStatus::Error);
        return;
    }

    m_sampler = llama_sampler_chain_init(llama_sampler_chain_default_params());
    llama_sampler_chain_add(m_sampler, llama_sampler_init_top_k(mLLMConfig->topK));
    llama_sampler_chain_add(m_sampler, llama_sampler_init_top_p(mLLMConfig->topP, 1));
    llama_sampler_chain_add(m_sampler, llama_sampler_init_temp(mLLMConfig->temperature));
    // llama_sampler_chain_add(m_sampler, llama_sampler_init_penalties(mLLMConfig->repeatPenalty, 0.0f, 0.0f));
    llama_sampler_chain_add(m_sampler, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));

    emit modelStatusChanged(EngineStatus::Ready);
}

LlamaLlmEngine::~LlamaLlmEngine() {
    unloadModel();
}

void LlamaLlmEngine::setConfig(QSharedPointer<LLMConfig> config){
    mLLMConfig = config;
}

void LlamaLlmEngine::unloadModel() {
    m_abort.store(true);
    if (m_sampler) { llama_sampler_free(m_sampler); m_sampler = nullptr; }
    if (m_ctx) { llama_free(m_ctx); m_ctx = nullptr; }
    if (m_model) { llama_model_free(m_model); m_model = nullptr; }
    emit modelStatusChanged(LiveTextify::EngineStatus::Idle);
}

void LlamaLlmEngine::generate(const QString &prompt) {
    if (!m_ctx || !m_model || !m_sampler) return;

    Logger::info(QString("LlamaLlmEngine: Generating response (prompt length: %1 chars)").arg(prompt.length()));
    Logger::debug(QString("Prompt: %1").arg(prompt.left(200) + (prompt.length() > 200 ? "..." : "")));

    emit generationBusyChanged(true);
    m_abort.store(false);

    llama_memory_t mem = llama_get_memory(m_ctx);
    if (mem) {
        llama_memory_clear(mem, true);
    }

    const llama_vocab *vocab = llama_model_get_vocab(m_model);
    std::string text = prompt.toStdString();

    std::vector<llama_token> tokens(text.size() + 32);
    int n = llama_tokenize(vocab, text.c_str(), text.size(), tokens.data(), tokens.size(), true, true);
    if (n < 0) {
        emit errorEncountered("Failed to tokenize prompt");
        emit generationBusyChanged(false);
        return;
    }
    tokens.resize(n);

    // Create batch with proper sequence IDs
    llama_batch batch = llama_batch_init(2048, 0, 1);
    for (int i = 0; i < n; ++i) {
        batch.token[i] = tokens[i];
        batch.pos[i] = i;
        batch.n_seq_id[i] = 1;
        batch.seq_id[i][0] = 0;
        batch.logits[i] = (i == n - 1);
    }
    batch.n_tokens = n;

    if (llama_decode(m_ctx, batch) != 0) {
        emit errorEncountered("Initial llama_decode failed");
        llama_batch_free(batch);
        emit generationBusyChanged(false);
        return;
    }

    int cur = n;
    int tokenCount = 0;
    while (!m_abort.load()) {
        QCoreApplication::processEvents();

        if (mLLMConfig->maxTokens > 0 && tokenCount >= mLLMConfig->maxTokens) break;

        llama_token id = llama_sampler_sample(m_sampler, m_ctx, -1);
        llama_sampler_accept(m_sampler, id);

        if (llama_vocab_is_eog(vocab, id)) break;

        char buf[128];
        int len = llama_token_to_piece(vocab, id, buf, sizeof(buf), 0, true);
        if (len > 0){
            emit tokenGenerated(QString::fromUtf8(buf, len));
            tokenCount++;
        }

        batch.n_tokens = 1;
        batch.token[0] = id;
        batch.pos[0] = cur++;
        batch.logits[0] = true;

        if (llama_decode(m_ctx, batch) != 0) {
            emit errorEncountered("Inference decode failed");
            break;
        }
    }

    llama_batch_free(batch);
    emit generationBusyChanged(false);
}
void LlamaLlmEngine::stop() { m_abort.store(true); }

void LlamaLlmEngine::reset() { m_abort.store(false); }
