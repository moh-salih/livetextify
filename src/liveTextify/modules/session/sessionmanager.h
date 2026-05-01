#pragma once
#include <QObject>
#include <QtWhisper/Types.h>
#include <QtLlama/Types.h>
#include "liveTextify/core/AppError.h"

class SessionService;
class TranscriptionService;
class ChatService;
class DatabaseService;
class SettingsManager;

class SessionManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(SessionService* sessionService   READ sessionService    CONSTANT)
    Q_PROPERTY(bool            isRecording      READ isRecording       NOTIFY isRecordingChanged)
    Q_PROPERTY(bool            isLlamaGenerating READ isLlamaGenerating NOTIFY isLlamaGeneratingChanged)
    Q_PROPERTY(int             whisperStatus    READ whisperStatus     NOTIFY whisperStatusChanged)
    Q_PROPERTY(int             llamaStatus      READ llamaStatus       NOTIFY llamaStatusChanged)
    Q_PROPERTY(int             embedderStatus   READ embedderStatus    NOTIFY embedderStatusChanged)
    Q_PROPERTY(SessionStatus   sessionStatus    READ sessionStatus     NOTIFY sessionStatusChanged)
    Q_PROPERTY(bool            hasError         READ hasError          NOTIFY lastErrorChanged)
    Q_PROPERTY(int             lastError        READ lastError         NOTIFY lastErrorChanged)
    Q_PROPERTY(QString         lastErrorString  READ lastErrorString   NOTIFY lastErrorChanged)

public:
    enum class SessionStatus { Idle, Recording, Paused };
    Q_ENUM(SessionStatus)

    explicit SessionManager(SettingsManager* settings, QObject* parent = nullptr);
    ~SessionManager();

    SessionService* sessionService()    const { return mSessionService; }

    bool          isRecording()         const { return mIsRecording; }
    bool          isLlamaGenerating()   const { return mIsLlamaGenerating; }
    int           whisperStatus()       const { return static_cast<int>(mWhisperStatus); }
    int           llamaStatus()         const { return static_cast<int>(mLlamaStatus); }
    int           embedderStatus()      const { return static_cast<int>(mEmbedderStatus); }
    SessionStatus sessionStatus()       const { return mSessionStatus; }
    bool          hasError()            const { return mHasError; }
    int           lastError()           const { return static_cast<int>(mLastError); }
    QString       lastErrorString()     const { return LiveTextify::appErrorToString(mLastError); }

    Q_INVOKABLE void clearError();

public slots:
    void startRecording();
    void stopRecording();
    void toggleRecording();
    void stopGeneration();

signals:
    void isRecordingChanged();
    void isLlamaGeneratingChanged();
    void whisperStatusChanged();
    void llamaStatusChanged();
    void embedderStatusChanged();
    void sessionStatusChanged();
    void lastErrorChanged();

private:
    void setSessionStatus(SessionStatus s);
    void reportError(LiveTextify::AppError error);

    DatabaseService*      mDatabaseService;
    SessionService*       mSessionService;
    TranscriptionService* mTranscriptionService;
    ChatService*          mChatService;

    bool              mIsRecording        = false;
    bool              mIsLlamaGenerating  = false;
    QtWhisper::Status mWhisperStatus      = QtWhisper::Status::Idle;
    QtLlama::Status   mLlamaStatus        = QtLlama::Status::Idle;
    QtLlama::Status   mEmbedderStatus     = QtLlama::Status::Idle;
    SessionStatus     mSessionStatus      = SessionStatus::Idle;
    bool                    mHasError  = false;
    LiveTextify::AppError   mLastError = LiveTextify::AppError::SttModelPathEmpty;
};
