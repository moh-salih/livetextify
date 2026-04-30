#pragma once

#include <QObject>
#include <QString>

#include <QtWhisper/Types.h>
#include <QtLlama/Types.h>
#include <QtAudioCapture/Types.h>
#include <QtRag/RagConfig.h>

// ─────────────────────────────────────────────────────────────────────────────
// AppError — the single error type exposed to QML / the UI layer.
//
// Each value encodes both the source subsystem and the failure kind so the UI
// can show a specific, actionable message without string parsing.
// ─────────────────────────────────────────────────────────────────────────────

namespace LiveTextify {
Q_NAMESPACE

enum class AppError {
    // ── Audio capture ────────────────────────────────────────────────────
    AudioDeviceUnavailable,
    AudioFormatUnsupported,
    AudioRecordingStartFailed,
    AudioDecodingFailed,
    AudioConfigNotSet,

    // ── Speech-to-text (Whisper) ─────────────────────────────────────────
    SttModelPathEmpty,
    SttModelLoadFailed,
    SttInferenceFailed,

    // ── LLM (llama) ──────────────────────────────────────────────────────
    LlmModelPathEmpty,
    LlmModelLoadFailed,
    LlmContextInitFailed,
    LlmTokenizationFailed,
    LlmDecodeFailed,

    // ── Embedder (llama embedding) ───────────────────────────────────────
    EmbModelPathEmpty,
    EmbModelLoadFailed,
    EmbContextInitFailed,
    EmbTokenizationFailed,
    EmbDecodeFailed,
    EmbRetrieveFailed,

    // ── RAG ──────────────────────────────────────────────────────────────
    RagEmbeddingFailed,
    RagStorageFailed,
    RagIndexSearchFailed,
};
Q_ENUM_NS(AppError)

// ── Mapping helpers ───────────────────────────────────────────────────────

inline AppError fromWhisperError(QtWhisper::Error e) {
    switch (e) {
    case QtWhisper::Error::ModelPathEmpty:  return AppError::SttModelPathEmpty;
    case QtWhisper::Error::ModelLoadFailed: return AppError::SttModelLoadFailed;
    case QtWhisper::Error::InferenceFailed: return AppError::SttInferenceFailed;
    default:                                return AppError::SttInferenceFailed;
    }
}

// QtLlama::Error is shared between Session (LLM) and Embedder.
// The caller must specify which subsystem the error came from.
inline AppError fromLlamaError(QtLlama::Error e, bool isEmbedder = false) {
    if (isEmbedder) {
        switch (e) {
        case QtLlama::Error::ModelPathEmpty:          return AppError::EmbModelPathEmpty;
        case QtLlama::Error::ModelLoadFailed:         return AppError::EmbModelLoadFailed;
        case QtLlama::Error::ContextInitFailed:       return AppError::EmbContextInitFailed;
        case QtLlama::Error::TokenizationFailed:      return AppError::EmbTokenizationFailed;
        case QtLlama::Error::DecodeFailed:            return AppError::EmbDecodeFailed;
        case QtLlama::Error::EmbeddingRetrieveFailed: return AppError::EmbRetrieveFailed;
        default:                                      return AppError::EmbDecodeFailed;
        }
    } else {
        switch (e) {
        case QtLlama::Error::ModelPathEmpty:    return AppError::LlmModelPathEmpty;
        case QtLlama::Error::ModelLoadFailed:   return AppError::LlmModelLoadFailed;
        case QtLlama::Error::ContextInitFailed: return AppError::LlmContextInitFailed;
        case QtLlama::Error::TokenizationFailed:return AppError::LlmTokenizationFailed;
        case QtLlama::Error::DecodeFailed:      return AppError::LlmDecodeFailed;
        default:                                return AppError::LlmDecodeFailed;
        }
    }
}

inline AppError fromAudioError(QtAudioCapture::Error e) {
    switch (e) {
    case QtAudioCapture::Error::DeviceUnavailable:    return AppError::AudioDeviceUnavailable;
    case QtAudioCapture::Error::FormatUnsupported:    return AppError::AudioFormatUnsupported;
    case QtAudioCapture::Error::RecordingStartFailed: return AppError::AudioRecordingStartFailed;
    case QtAudioCapture::Error::DecodingFailed:       return AppError::AudioDecodingFailed;
    case QtAudioCapture::Error::ConfigNotSet:         return AppError::AudioConfigNotSet;
    default:                                          return AppError::AudioDeviceUnavailable;
    }
}

inline AppError fromRagError(QtRag::Error e) {
    switch (e) {
    case QtRag::Error::EmbeddingFailed:   return AppError::RagEmbeddingFailed;
    case QtRag::Error::StorageFailed:     return AppError::RagStorageFailed;
    case QtRag::Error::IndexSearchFailed: return AppError::RagIndexSearchFailed;
    default:                              return AppError::RagEmbeddingFailed;
    }
}

// ── UI-facing string ──────────────────────────────────────────────────────

inline QString appErrorToString(AppError e) {
    switch (e) {
    case AppError::AudioDeviceUnavailable:    return "No audio input device found.";
    case AppError::AudioFormatUnsupported:    return "Audio format is not supported by this device.";
    case AppError::AudioRecordingStartFailed: return "Failed to start audio recording.";
    case AppError::AudioDecodingFailed:       return "Audio file could not be decoded.";
    case AppError::AudioConfigNotSet:         return "Audio configuration was not set before recording.";

    case AppError::SttModelPathEmpty:         return "Speech-to-text model path is not set.";
    case AppError::SttModelLoadFailed:        return "Failed to load speech-to-text model.";
    case AppError::SttInferenceFailed:        return "Speech recognition failed.";

    case AppError::LlmModelPathEmpty:         return "Language model path is not set.";
    case AppError::LlmModelLoadFailed:        return "Failed to load language model.";
    case AppError::LlmContextInitFailed:      return "Failed to initialize language model context.";
    case AppError::LlmTokenizationFailed:     return "Failed to tokenize input.";
    case AppError::LlmDecodeFailed:           return "Language model inference failed.";

    case AppError::EmbModelPathEmpty:         return "Embedding model path is not set.";
    case AppError::EmbModelLoadFailed:        return "Failed to load embedding model.";
    case AppError::EmbContextInitFailed:      return "Failed to initialize embedding model context.";
    case AppError::EmbTokenizationFailed:     return "Failed to tokenize text for embedding.";
    case AppError::EmbDecodeFailed:           return "Embedding decode failed.";
    case AppError::EmbRetrieveFailed:         return "Could not retrieve embedding vector from model.";

    case AppError::RagEmbeddingFailed:        return "RAG embedding generation failed.";
    case AppError::RagStorageFailed:          return "Failed to save chunk to RAG storage.";
    case AppError::RagIndexSearchFailed:      return "RAG vector search encountered an error.";

    default:                                  return "An unknown error occurred.";
    }
}

} // namespace LiveTextify

Q_DECLARE_METATYPE(LiveTextify::AppError)
