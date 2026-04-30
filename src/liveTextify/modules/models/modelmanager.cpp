#include "liveTextify/modules/models/modelmanager.h"

ModelManager::ModelManager(QObject* parent) : QObject(parent) {
    mStt = new SttModelManager(this);
    mLlm = new LlmModelManager(this);
    mEmb = new EmbModelManager(this);

    connect(mStt, &ModelTypeManager::selectedPathChanged, this,
            [this](const QString& path) { emit defaultModelChanged("stt", path); });
    connect(mLlm, &ModelTypeManager::selectedPathChanged, this,
            [this](const QString& path) { emit defaultModelChanged("llm", path); });
    connect(mEmb, &ModelTypeManager::selectedPathChanged, this,
            [this](const QString& path) { emit defaultModelChanged("emb", path); });
}

ModelManager::~ModelManager() = default;
