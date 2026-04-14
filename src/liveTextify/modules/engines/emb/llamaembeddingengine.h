#pragma once
#include <llama.h>
#include <atomic>

#include "liveTextify/interfaces/IEmbeddingEngine.h"

class LlamaEmbeddingEngine : public IEmbeddingEngine {
    Q_OBJECT
public:
    explicit LlamaEmbeddingEngine(QObject *parent = nullptr);
    ~LlamaEmbeddingEngine() override;

public slots:
    void loadModel() override;
    void unloadModel() override;
    void generateEmbedding(const QString &text, int chunkIndex) override;
    void setConfig(QSharedPointer<EmbConfig> config) override;
    void stop() override;
    void reset() override;

private:
    llama_model               * m_model   = nullptr;
    llama_context             * m_ctx     = nullptr;

    std::atomic<bool>           m_abort{false};
    QSharedPointer<EmbConfig>   mEmbConfig;
};
