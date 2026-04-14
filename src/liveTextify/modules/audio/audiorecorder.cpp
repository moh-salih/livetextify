#include "liveTextify/modules/audio/audiorecorder.h"
#include "liveTextify/modules/audio/audioutils.h"
#include "liveTextify/modules/engines/common.h"
#include "liveTextify/core/Logger.h"


AudioRecorder::AudioRecorder(QObject *parent): IAudioSource{parent}, mAudioSource(nullptr), mIO(nullptr){

}

void AudioRecorder::setConfig(const AudioConfig &config) {
    QAudioFormat format;
    format.setSampleRate(config.audioSampleRate);
    format.setChannelCount(config.audioChannels);
    format.setSampleFormat(QAudioFormat::Int16); // Whisper requires Int16 PCM
    setFormat(format);
}

void AudioRecorder::setFormat(const QAudioFormat &format) {
    mAudioFormat = format;
}


bool AudioRecorder::start(){
    if(mRunning) return false;

    // Delete previous object safely
    if(mAudioSource) {
        mAudioSource->stop();
        mAudioSource->deleteLater();
    }

    // Check Audio Input device if exists
    QAudioDevice device = QMediaDevices::defaultAudioInput();
    if (device.isNull()) {
        Logger::warning("No audio input device found!");
        return false;
    }

    Logger::info(QString("[AudioRecorder] Using audio device: %1").arg(device.description()));
    Logger::info(QString("[AudioRecorder] Device preferred format: rate=%1, channels=%2, sampleFormat=%3")
        .arg(device.preferredFormat().sampleRate())
        .arg(device.preferredFormat().channelCount())
        .arg((int)device.preferredFormat().sampleFormat())
    );


    mAudioSource = new QAudioSource(mAudioFormat, this);
    mAudioFormat = mAudioSource->format();


    Logger::info(QString("[AudioRecorder] Actual format: rate=%1, channels=%2, sampleFormat=%3")
        .arg(mAudioFormat.sampleRate())
        .arg(mAudioFormat.channelCount())
        .arg((int)mAudioFormat.sampleFormat())
    );

    emit formatChanged(mAudioFormat);

    mIO = mAudioSource->start();

    if (!mIO) {
        Logger::error("[AudioRecorder] Failed to start audio source!");
        return false;
    }

    connect(mIO, &QIODevice::readyRead, this, [this]{
        QByteArray chunk = mIO->readAll();
        if(!chunk.isEmpty()){
            m_audioLevel = AudioUtils::calculateRmsLevel(chunk);
            emit audioLevelChanged();
            emit audioChunk(chunk);
        }
    });


    Logger::debug(QString("Actual input format: Rate = %1, Channels = %2, Format = %3")
        .arg(mAudioFormat.sampleRate())
        .arg(mAudioFormat.channelCount())
        .arg((int)mAudioFormat.sampleFormat())
    );

    mRunning = true;
    emit runningChanged(mRunning);
    emit started();
    return true;
}



void AudioRecorder::stop(){
    if(mAudioSource){
        mAudioSource->stop();
        mAudioSource->deleteLater();
        mAudioSource = nullptr;
    }

    if(mIO){
        QObject::disconnect(mIO, nullptr, this, nullptr);
        mIO = nullptr;
    }



    mRunning = false;
    emit runningChanged(mRunning);
    emit stopped();

    Logger::debug("[AudioRecorder] Recording stopped.");
}

QAudioFormat AudioRecorder::audioFormat() const {
    return mAudioFormat;
}



