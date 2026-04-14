#include <QtMath>
#include "liveTextify/modules/audio/audioutils.h"


float AudioUtils::calculateRmsLevel(const QByteArray &data) {
    if (data.isEmpty()) return 0.0f;

    const int16_t *samples = reinterpret_cast<const int16_t*>(data.constData());
    int sampleCount = data.size() / sizeof(int16_t);

    double sumOfSquares = 0;
    for (int i = 0; i < sampleCount; ++i) {
        double normalized = samples[i] / 32768.0;
        sumOfSquares += normalized * normalized;
    }

    double rms = std::sqrt(sumOfSquares / sampleCount);
    return static_cast<float>(rms);
}
