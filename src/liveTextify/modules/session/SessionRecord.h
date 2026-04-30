#pragma once
#include <QString>
#include <QDateTime>

struct SessionRecord {
    int           dbId      = -1;
    QString       title;
    QDateTime     createdAt;
    QString       systemPrompt;
};
