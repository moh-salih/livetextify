#include "sttsession.h"

using namespace LiveTextify;

SttSession::SttSession(QObject *parent): QObject(parent) {}

SttSession::~SttSession() {
    if (mWorkerThread) {
        mWorkerThread->quit();
        mWorkerThread->wait();
    }
}

void SttSession::initialize(ISttEngine* engine) {
    if (mWorkerThread) return;

    mEngine = engine;
    mWorkerThread = new QThread(this);
    mEngine->setParent(nullptr);
    mEngine->moveToThread(mWorkerThread);
    mEngine->setConfig(mSTTConfig);

    connect(mEngine, &ISttEngine::transcriptionSegmentReady, this, &SttSession::onPartialResult);
    connect(mEngine, &ISttEngine::modelStatusChanged, this, [this](EngineStatus s){
        if (mStatus != (Status)s) {
            mStatus = (Status)s;
            emit statusChanged();
        }
    });


    connect(mWorkerThread, &QThread::finished, mEngine, &QObject::deleteLater);
    mWorkerThread->start();
}

void SttSession::setConfig(const STTConfig& sttConfig) {
    *mSTTConfig = sttConfig;
}

void SttSession::loadModel() {
    if (!mSTTConfig || mSTTConfig->modelPath.isEmpty()) return;
    QMetaObject::invokeMethod(mEngine, "loadModel");
}

void SttSession::unloadModel() {
    stopInference();
    QMetaObject::invokeMethod(mEngine, "unloadModel");
}

void SttSession::processAudioWindow(const std::vector<float> &samples) {
    if (mStatus != Status::Ready || samples.empty()) return;
    QMetaObject::invokeMethod(mEngine, "processWindow", Q_ARG(std::vector<float>, samples));
}

void SttSession::startInference() {
    if (mStatus != Status::Ready) return;
    QMetaObject::invokeMethod(mEngine, "reset");
}

void SttSession::stopInference() {
    QMetaObject::invokeMethod(mEngine, "stop");
}

void SttSession::onPartialResult(const QString& text) {
    if (text.isEmpty()) return;
    mTranscription += (mTranscription.isEmpty() ? "" : " ") + text;
    emit transcriptionChanged(mTranscription);
    emit partialResult(text);
}

void SttSession::clear() {
    mTranscription.clear();
    emit transcriptionChanged(mTranscription);
}

QString SttSession::statusText() const {
    switch ((EngineStatus)mStatus) {
        case EngineStatus::Idle:    return tr("No Model");
        case EngineStatus::Loading: return tr("Loading...");
        case EngineStatus::Ready:   return tr("Ready");
        case EngineStatus::Error:   return tr("Error");
        default:                    return tr("Unknown");
    }        
}
