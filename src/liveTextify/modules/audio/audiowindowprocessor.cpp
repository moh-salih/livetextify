#include "liveTextify/modules/audio/audiowindowprocessor.h"
#include "liveTextify/modules/engines/common.h"

#include "liveTextify/core/Logger.h"

#include <algorithm>

#include <chrono>

using namespace std::chrono_literals;


AudioWindowProcessor::AudioWindowProcessor(QObject *parent): QObject(parent) {

}

void AudioWindowProcessor::reset() {
    m_buffer.clear();
    m_prevTail.clear();
}


void AudioWindowProcessor::setConfig(const AudioConfig &config) {
    mSamplesStep = (config.audioSampleRate * config.step)      / 1s;
    mSamplesKeep = (config.audioSampleRate * config.keep)      / 1s;
    mSamplesMax  = (config.audioSampleRate * config.maxWindow) / 1s;
    m_buffer.clear();
    m_buffer.reserve(mSamplesMax);
    m_prevTail.clear();
    m_prevTail.reserve(mSamplesKeep);
}

void AudioWindowProcessor::handleChunk(const QByteArray &pcm) {

    if (pcm.isEmpty()) return;

    // 1. Convert incoming PCM16 to float
    std::vector<float> newSamples = pcm16ToFloat(pcm);

    // 2. Guard BEFORE inserting: drop oldest samples if falling behind
    if (m_buffer.size() + newSamples.size() > mSamplesMax) {
        size_t excess = (m_buffer.size() + newSamples.size()) - mSamplesMax;
        excess = std::min(excess, m_buffer.size());
        m_buffer.erase(m_buffer.begin(), m_buffer.begin() + excess);
        Logger::warning(QString("AudioWindowProcessor: buffer overflow, dropped % samples").arg(excess));
    }

    // 3. Append to accumulation buffer
    m_buffer.insert(m_buffer.end(), newSamples.begin(), newSamples.end());

    // 4. Process all available steps
    while (m_buffer.size() >= mSamplesStep) {
        // Assemble the window: [overlap context] + [new step]
        std::vector<float> window;
        window.reserve(m_prevTail.size() + mSamplesStep);
        window.insert(window.end(), m_prevTail.begin(), m_prevTail.end());

        auto stepEnd = m_buffer.begin() + mSamplesStep;
        window.insert(window.end(), m_buffer.begin(), stepEnd);

        // 5. Remove the processed step first...
        m_buffer.erase(m_buffer.begin(), stepEnd);

        // 6. ...then take tail from remaining buffer end (unconditionally correct)
        size_t keepStart = m_buffer.size() >= mSamplesKeep ? m_buffer.size() - mSamplesKeep : 0;
        m_prevTail.assign(m_buffer.begin() + keepStart, m_buffer.end());

        // 7. Prevent silent windows from sending to whisper
        float rms = 0.0f;
        for (float s : window) rms += s * s;
        rms = std::sqrt(rms / window.size());

        // static constexpr float kSilenceThreshold = 0.01f; // tune this
        // if (rms < kSilenceThreshold) return; // skip silent windows


        // 8. Emit
        emit windowReady(window);
    }
}

std::vector<float> AudioWindowProcessor::pcm16ToFloat(const QByteArray &pcm) {
    size_t sampleCount = pcm.size() / sizeof(int16_t);
    std::vector<float> out;
    out.reserve(sampleCount);

    const int16_t *samples = reinterpret_cast<const int16_t*>(pcm.constData());

    for (size_t i = 0; i < sampleCount; ++i) {
        // Normalize int16 range [-32768, 32767] to float [-1.0, 1.0]
        out.push_back(static_cast<float>(samples[i]) / 32768.0f);
    }

    return out;
}

