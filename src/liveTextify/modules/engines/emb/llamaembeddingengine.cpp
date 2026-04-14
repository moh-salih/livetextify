#include "liveTextify/modules/engines/emb/llamaembeddingengine.h"

#include "liveTextify/core/Logger.h"

#include <QCoreApplication>


using namespace LiveTextify;

LlamaEmbeddingEngine::LlamaEmbeddingEngine(QObject *parent) : IEmbeddingEngine(parent) {
    static bool backendInit = false;
    if (!backendInit) {
        llama_backend_init();
        backendInit = true;
    }
    // Quiet the llama logs for embeddings
    // llama_log_set([](enum ggml_log_level, const char*, void*){}, NULL);
}

LlamaEmbeddingEngine::~LlamaEmbeddingEngine() {
    unloadModel();
}

void LlamaEmbeddingEngine::loadModel() {
    unloadModel(); // Ensure cleanup before new load
    emit modelStatusChanged(EngineStatus::Loading);

    if (!mEmbConfig || mEmbConfig->modelPath.isEmpty()) {
        Logger::error("LlamaEmbedding: Model path is not set in configuration.");
        emit errorEncountered(tr("Failed to load embedding model: Path is empty."));
        emit modelStatusChanged(EngineStatus::Error);
        return;
    }


    Logger::info("=== LLAMA EMBEDDING ENGINE LOADING ===");
    Logger::info(QString("  Model Path: %1").arg(mEmbConfig->modelPath));
    Logger::info(QString("  Context Size (nCtx): %1").arg(mEmbConfig->nCtx));
    Logger::info(QString("  Thread Count: %1").arg(mEmbConfig->nThreads));
    Logger::info(QString("  GPU Layers: %1").arg(mEmbConfig->nGpuLayers));

    llama_model_params mp = llama_model_default_params();
    mp.n_gpu_layers     = mEmbConfig->nGpuLayers;

    m_model = llama_model_load_from_file(mEmbConfig->modelPath.toStdString().c_str(), mp);
    if (!m_model) {
        Logger::error(QString("LlamaEmbedding: Failed to load model from %1").arg(mEmbConfig->modelPath));
        emit errorEncountered(tr("Failed to load embedding model file."));
        emit modelStatusChanged(EngineStatus::Error);
        return;
    }

    llama_context_params cp = llama_context_default_params();
    cp.n_ctx            = mEmbConfig->nCtx;
    cp.n_threads        = mEmbConfig->nThreads;
    cp.embeddings = true; // Enables vector output instead of text generation

    m_ctx = llama_init_from_model(m_model, cp);
    if (!m_ctx) {
        spdlog::error("LlamaEmbedding: Failed to init context");
        emit modelStatusChanged(EngineStatus::Error);
        return;
    }

    emit modelStatusChanged(EngineStatus::Ready);
}

void LlamaEmbeddingEngine::generateEmbedding(const QString &text, int chunkIndex) {
    if (!m_ctx || !m_model) return;

    Logger::debug(QString("LlamaEmbeddingEngine: Generating embedding for chunk %1 (text length: %2 chars)").arg(chunkIndex).arg(text.length()));
    emit computeBusyChanged(true);
    m_abort.store(false);

    // llama_kv_cache_clear(m_ctx);

    std::string stdText = text.toStdString();
    const llama_vocab *vocab = llama_model_get_vocab(m_model);

    // 1. Tokenize the input text
    std::vector<llama_token> tokens(stdText.size() + 32);
    int n_tokens = llama_tokenize(vocab, stdText.c_str(), stdText.size(), tokens.data(), tokens.size(), true, true);

    if (n_tokens < 0) {
        emit errorEncountered(tr("Tokenization failed for embedding chunk."));
        emit computeBusyChanged(false);
        return;
    }

    // 2. Prepare the batch for processing
    llama_batch batch = llama_batch_get_one(tokens.data(), n_tokens);

    // 3. Decode (Forward pass)
    if (llama_decode(m_ctx, batch) != 0) {
        emit errorEncountered(tr("Llama decode failed during embedding calculation."));
        emit computeBusyChanged(false);
        return;
    }

    // 4. Retrieve the vector from the model's embedding layer
    const int n_embd = llama_model_n_embd(m_model);
    const float *embd = llama_get_embeddings_seq(m_ctx, 0);

    if (embd) {
        std::vector<float> vector;
        vector.assign(embd, embd + n_embd);
        // Success: Send vector back to RagService via the Session
        emit vectorReady(vector, text, chunkIndex);
    } else {
        Logger::error("LlamaEmbedding: Retrieve failed. Check model compatibility.");
        emit errorEncountered(tr("Could not retrieve embedding vector."));
    }

    emit computeBusyChanged(false);
}

void LlamaEmbeddingEngine::setConfig(QSharedPointer<EmbConfig> config){
    mEmbConfig = config;
}

void LlamaEmbeddingEngine::unloadModel() {
    m_abort.store(true);
    if (m_ctx) {
        llama_free(m_ctx);
        m_ctx = nullptr;
    }
    if (m_model) {
        llama_model_free(m_model);
        m_model = nullptr;
    }
    emit modelStatusChanged(EngineStatus::Idle);
}

void LlamaEmbeddingEngine::stop() {
    m_abort.store(true);
}

void LlamaEmbeddingEngine::reset() {
    m_abort.store(false);
}
