#pragma once
#include <QObject>

class SessionService;
class AiService;
class AudioService;
class RagService;
class ConversationService;
class QuestionService;
class DatabaseService;
class SettingsManager;

class SessionManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(SessionService      * sessions  READ sessionService      CONSTANT)
    Q_PROPERTY(AiService           * ai        READ aiService           CONSTANT)
    Q_PROPERTY(AudioService        * audio     READ audioService        CONSTANT)
    Q_PROPERTY(RagService          * rag       READ ragService          CONSTANT)
    Q_PROPERTY(ConversationService * chat      READ conversationService CONSTANT)
    Q_PROPERTY(QuestionService     * questions READ questionService     CONSTANT)

public:
    explicit SessionManager(SettingsManager * settings, QObject *parent = nullptr);
    ~SessionManager();

    SessionService      * sessionService()      const { return mSessionService; }
    AiService           * aiService()           const { return mAiService; }
    AudioService        * audioService()        const { return mAudioService; }
    RagService          * ragService()          const { return mRagService; }
    ConversationService * conversationService() const { return mConversationService; }
    QuestionService     * questionService()     const { return mQuestionService; }

private:
    SessionService      * mSessionService;
    AiService           * mAiService;
    AudioService        * mAudioService;
    RagService          * mRagService;
    ConversationService * mConversationService;
    QuestionService     * mQuestionService;
    DatabaseService     * mDatabaseService;
};
