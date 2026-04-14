#pragma once

#include <QString>
#include <QDateTime>

struct Message {
    QString role;
    QString content;
    QDateTime timestamp;
    bool isComplete;

    Message() = default;
    Message(const QString& r, const QString& c)
        : role(r), content(c), timestamp(QDateTime::currentDateTime()), isComplete(true) {}

    Message(const QString& r, const QString& c, bool complete)
        : role(r), content(c), timestamp(QDateTime::currentDateTime()), isComplete(complete) {}
};
