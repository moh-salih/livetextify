#pragma once
#include <QObject>
#include <QQmlEngine>
#include <thread>
#include <algorithm>
#include <chrono>

using namespace std::chrono_literals;


namespace LiveTextify {
    Q_NAMESPACE
    enum class EngineStatus { Idle, Loading, Ready, Error };
    Q_ENUM_NS(EngineStatus);

    enum class BusyState { Idle, Processing };
    Q_ENUM_NS(BusyState)
}


struct EmbConfig {
    QString         modelPath;
    int             nCtx                    = 2048;
    int             nThreads                = 1;
    int             nGpuLayers              = 0;
};

struct LLMConfig {
    QString         modelPath;
    QString         systemPrompt            = "You are a helpful assistant, your name is LiveTextify";
    int             threadCount             = 1;
    int             batchThreads            = 2;
    int             contextLength           = 4096;
    int             batchSize               = 2048;
    float           temperature             = 0.7f;
    float           topP                    = 0.9f;
    int             topK                    = 40;
    float           repeatPenalty           = 1.1f;
    int             maxTokens               = -1;
};

struct AudioConfig {
    using milliseconds  = std::chrono::milliseconds;

    int             audioSampleRate         = 16000;
    int             audioChannels           = 1;
    milliseconds    step                    = 1s;
    milliseconds    keep                    = 500ms;
    milliseconds    maxWindow               = 15s;
};

struct STTConfig {
    QString         modelPath;
    bool            useGpu                  = true;
    QString         language                = "en";
    bool            autoDetectLanguage      = false;
    bool            translate               = false;
    int             maxTokens               = 256;
    int             threadCount             = std::min(4, static_cast<int>(std::thread::hardware_concurrency()));
    float           temperature             = 0.0f;
};

struct RAGConfig {
    bool            enabled                 = true;
    int             minChars                = 50;
    int             maxChars                = 450;
    int             topK                    = 5;
    int             dim                     = 768;
    float           minScore                = 0.35f;
    float           lengthBoost             = 0.15f;
    float           fillerLimit             = 0.25f;
};
