#pragma once

#include <QObject>
#include <QByteArray>
#include <vector>

struct AudioConfig;
class AudioWindowProcessor : public QObject {
    Q_OBJECT

public:
    explicit AudioWindowProcessor(QObject *parent = nullptr);

    // Resets all internal buffers. Call this when stopping/starting recording.
    void reset();

    void setConfig(const AudioConfig& config);

public slots:
    // Receives a raw PCM16 chunk from the AudioRecorder.
    void handleChunk(const QByteArray &pcm);

signals:
    // Emitted when a full window (context + new step) is ready for transcription.
    void windowReady(const std::vector<float> &samples);

private:
    size_t mSamplesStep = 0;
    size_t mSamplesKeep = 0;
    size_t mSamplesMax  = 0;


    std::vector<float> m_buffer;
    std::vector<float> m_prevTail;

    // Converts PCM16 (int16_t) to Normalized Float (float).
    std::vector<float> pcm16ToFloat(const QByteArray &pcm);
};
