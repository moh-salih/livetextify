#pragma once
#include <QObject>
#include <QByteArray>

namespace {{projectName}} {
    class IDumpInterface  {
    public:
        virtual ~IDumpInterface() = default;
        virtual void dumpData(const QByteArray& data) = 0;
    };
}

