#include "llmsession.h"
#include <QThread>

#include "liveTextify/interfaces/ILlmEngine.h"

using namespace LiveTextify;

LlmSession::LlmSession(QObject *parent) : QObject(parent) {}

LlmSession::~LlmSession() {
    if (mWorkerThread) {
        mWorkerThread->quit();
        mWorkerThread->wait();
    }
}

void LlmSession::initialize(ILlmEngine* engine) {
    if (mWorkerThread) return;
    mEngine = engine;
    mWorkerThread = new QThread(this);

    mEngine->setParent(nullptr);
    mEngine->moveToThread(mWorkerThread);
    mEngine->setConfig(mLLMConfig);

    connect(mEngine, &ILlmEngine::tokenGenerated, this, [this](const QString& chunk){
        emit textGenerated(chunk, mCurrentSessionId);
    });

    connect(mEngine, &ILlmEngine::generationBusyChanged, this, [this](bool busy){
        if (mIsGenerating != busy) {
            mIsGenerating = busy;
            emit isGeneratingChanged(busy);
        }
    });

    connect(mEngine, &ILlmEngine::modelStatusChanged, this, [this](EngineStatus s){
        if (mStatus != s) {
            mStatus = s;
            emit statusChanged(mStatus);
        }
    });

    connect(mEngine, &ILlmEngine::errorEncountered, this, [this](const QString& msg){
        emit errorOccurred(msg);
    });

    connect(mWorkerThread, &QThread::finished, mEngine, &QObject::deleteLater);
    mWorkerThread->start();
}

void LlmSession::setConfig(const LLMConfig &config){
    *mLLMConfig = config;
}

void LlmSession::generate(const QString& prompt, int sessionId) {
    if (mStatus != EngineStatus::Ready) return;
    mCurrentSessionId = sessionId;
    QMetaObject::invokeMethod(mEngine, "generate", Q_ARG(QString, prompt));
}

void LlmSession::stop() {
    if (mEngine) QMetaObject::invokeMethod(mEngine, "stop");
}

void LlmSession::loadModel() {
    if (!mLLMConfig || mLLMConfig->modelPath.isEmpty()) return;
    QMetaObject::invokeMethod(mEngine, "loadModel");
}

void LlmSession::unloadModel() {
    if (mEngine) QMetaObject::invokeMethod(mEngine, "unloadModel");
}

QString LlmSession::statusText() const {
    switch (mStatus) {
        case EngineStatus::Idle:    return tr("No Model Loaded");
        case EngineStatus::Loading: return tr("Loading Model...");
        case EngineStatus::Ready:   return tr("Ready");
        case EngineStatus::Error:   return tr("Model Error");
        default:                    return tr("Unknown");
    }
}
