#pragma once
#include <QObject>
#include "liveTextify/modules/models/modeltypemanager.h"

class ModelManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(SttModelManager* stt READ stt CONSTANT)
    Q_PROPERTY(LlmModelManager* llm READ llm CONSTANT)
    Q_PROPERTY(EmbModelManager* emb READ emb CONSTANT)

public:
    explicit ModelManager(QObject* parent = nullptr);
    ~ModelManager();

    SttModelManager* stt() const { return mStt; }
    LlmModelManager* llm() const { return mLlm; }
    EmbModelManager* emb() const { return mEmb; }

signals:
    void defaultModelChanged(const QString& type, const QString& path);

private:
    SttModelManager* mStt;
    LlmModelManager* mLlm;
    EmbModelManager* mEmb;
};
