#pragma once
#include "liveTextify/interfaces/ISttEngine.h"
#include <whisper.h>
#include <atomic>

class WhisperSttEngine : public ISttEngine {
    Q_OBJECT
public:
    explicit WhisperSttEngine(QObject *parent = nullptr);
    ~WhisperSttEngine() override;

public slots:
    void setConfig(QSharedPointer<STTConfig> config) override;
    void loadModel() override;
    void unloadModel() override;
    void processWindow(const std::vector<float> &samples) override;
    void stop() override;
    void reset() override;

private:
    static bool abort_callback(void *user_data);

    whisper_context           * m_ctx = nullptr;
    std::atomic<bool>           m_abortRequested{false};
    QSharedPointer<STTConfig>   mSTTConfig;
};
