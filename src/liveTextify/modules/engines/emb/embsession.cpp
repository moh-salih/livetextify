#include "embsession.h"
#include "liveTextify/interfaces/IEmbeddingEngine.h"

using namespace LiveTextify;

EmbSession::EmbSession(QObject *parent) : QObject(parent) {}

EmbSession::~EmbSession() {
    if (mWorkerThread) {
        mWorkerThread->quit();
        mWorkerThread->wait();
    }
}

void EmbSession::initialize(IEmbeddingEngine* engine) {
    if (mWorkerThread) return;

    mEngine = engine;
    mWorkerThread = new QThread(this);
    mEngine->setParent(nullptr);
    mEngine->moveToThread(mWorkerThread);
    mEngine->setConfig(mEmbConfig);

    // Bridge Engine signals to Session signals
    connect(mEngine, &IEmbeddingEngine::vectorReady, this, &EmbSession::embeddingReady);

    connect(mEngine, &IEmbeddingEngine::computeBusyChanged, this, [this](bool busy){
        if (mIsGenerating != busy) {
            mIsGenerating = busy;
            emit isGeneratingChanged(busy);
        }
    });

    connect(mEngine, &IEmbeddingEngine::modelStatusChanged, this, [this](EngineStatus s){
        if (mStatus != s) {
            mStatus = s;
            emit statusChanged();
        }
    });

    connect(mEngine, &IEmbeddingEngine::errorEncountered, this, &EmbSession::errorOccurred);

    connect(mWorkerThread, &QThread::finished, mEngine, &QObject::deleteLater);
    mWorkerThread->start();
}

void EmbSession::setConfig(const EmbConfig &config){
    *mEmbConfig = config;
}

void EmbSession::generateEmbedding(const QString& text, int chunkIndex) {
    if (mStatus != EngineStatus::Ready) return;
    QMetaObject::invokeMethod(mEngine, "generateEmbedding", Q_ARG(QString, text), Q_ARG(int, chunkIndex));
}

void EmbSession::loadModel() {
    if (!mEmbConfig || mEmbConfig->modelPath.isEmpty()) return;
    QMetaObject::invokeMethod(mEngine, "loadModel");
}

void EmbSession::unloadModel() {
    QMetaObject::invokeMethod(mEngine, "unloadModel");
}

void EmbSession::stop() {
    if (mEngine) QMetaObject::invokeMethod(mEngine, "stop");
}

QString EmbSession::statusText() const {
    switch (mStatus) {
    case EngineStatus::Idle:    return tr("No Embedding Model");
    case EngineStatus::Loading: return tr("Loading Vector Engine...");
    case EngineStatus::Ready:   return tr("Ready");
    case EngineStatus::Error:   return tr("Vector Engine Error");
    default:                    return tr("Unknown");
    }
}
