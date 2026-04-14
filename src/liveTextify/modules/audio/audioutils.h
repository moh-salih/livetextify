#pragma once
#include <QByteArray>

class AudioUtils {
public:
    static float calculateRmsLevel(const QByteArray &data);
};

