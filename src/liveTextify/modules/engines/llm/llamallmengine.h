#pragma once
#include <llama.h>
#include <atomic>
#include "liveTextify/interfaces/ILlmEngine.h"

class LlamaLlmEngine : public ILlmEngine {
    Q_OBJECT
public:
    explicit LlamaLlmEngine(QObject *parent = nullptr);
    ~LlamaLlmEngine() override;

public slots:
    void setConfig(QSharedPointer<LLMConfig> config) override;
    void loadModel() override;
    void unloadModel() override;
    void generate(const QString &prompt) override;
    void stop() override;
    void reset() override;
private:
    llama_model                 * m_model   = nullptr;
    llama_context               * m_ctx     = nullptr;
    llama_sampler               * m_sampler = nullptr;

    QSharedPointer<LLMConfig>     mLLMConfig;

    std::atomic<bool>             m_abort{false};
};
