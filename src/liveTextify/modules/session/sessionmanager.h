#pragma once
#include <QObject>
#include <QtWhisper/Types.h>
#include <QtLlama/Types.h>
#include "liveTextify/core/AppError.h"

class SessionService;
class TranscriptionService;
class AudioService;
class ChatService;
class RagService;
class DatabaseService;

class SessionManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(SessionService* sessionService    READ sessionService    CONSTANT)
    Q_PROPERTY(bool            isRecording       READ isRecording       NOTIFY isRecordingChanged)
    Q_PROPERTY(bool            isTranscribing    READ isTranscribing    NOTIFY isTranscribingChanged)
    Q_PROPERTY(bool            isLlamaGenerating READ isLlamaGenerating NOTIFY isLlamaGeneratingChanged)
    Q_PROPERTY(int             whisperStatus     READ whisperStatus     NOTIFY whisperStatusChanged)
    Q_PROPERTY(int             llamaStatus       READ llamaStatus       NOTIFY llamaStatusChanged)
    Q_PROPERTY(int             embedderStatus    READ embedderStatus    NOTIFY embedderStatusChanged)
    Q_PROPERTY(SessionStatus   sessionStatus     READ sessionStatus     NOTIFY sessionStatusChanged)
    Q_PROPERTY(bool            hasError          READ hasError          NOTIFY lastErrorChanged)
    Q_PROPERTY(int             lastError         READ lastError         NOTIFY lastErrorChanged)
    Q_PROPERTY(QString         lastErrorString   READ lastErrorString   NOTIFY lastErrorChanged)

public:
    enum class SessionStatus { Idle, Recording, Paused };
    Q_ENUM(SessionStatus)

    explicit SessionManager(QObject* parent = nullptr);
    ~SessionManager();

    SessionService* sessionService()   const { return mSessionService; }

    // ── Direct service getters — no caching ───────────────────────────────────
    bool isRecording()       const;
    bool isTranscribing()    const;
    bool isLlamaGenerating() const;
    int  whisperStatus()     const;
    int  llamaStatus()       const;
    int  embedderStatus()    const;

    SessionStatus sessionStatus()   const { return mSessionStatus; }
    bool          hasError()        const { return mHasError; }
    int           lastError()       const { return static_cast<int>(mLastError); }
    QString       lastErrorString() const { return LiveTextify::appErrorToString(mLastError); }

    Q_INVOKABLE void clearError();

public slots:
    void startRecording();
    void stopRecording();
    void toggleRecording();
    void stopGeneration();

signals:
    void isRecordingChanged();
    void isTranscribingChanged();
    void isLlamaGeneratingChanged();
    void whisperStatusChanged();
    void llamaStatusChanged();
    void embedderStatusChanged();
    void sessionStatusChanged();
    void lastErrorChanged();

private:
    void setSessionStatus(SessionStatus s);
    void reportError(LiveTextify::AppError error);
    void wireReloadSignals();

    DatabaseService*      mDatabaseService;
    SessionService*       mSessionService;
    AudioService*         mAudioService;
    TranscriptionService* mTranscriptionService;
    RagService*           mRagService;
    ChatService*          mChatService;

    SessionStatus         mSessionStatus = SessionStatus::Idle;
    bool                  mHasError      = false;
    LiveTextify::AppError mLastError     = LiveTextify::AppError::SttModelPathEmpty;
};
