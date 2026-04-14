#pragma once

#include <QObject>
#include <QStringListModel>
#include "liveTextify/modules/services/aiservice.h"
#include "liveTextify/modules/services/sessionservice.h"

class QuestionService : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringListModel* questionsList READ questionsList NOTIFY questionsGenerated)

public:
    explicit QuestionService( AiService* aiService, SessionService* sessionService, QObject* parent = nullptr);

    QStringListModel* questionsList() const { return m_questionsList; }

    Q_INVOKABLE void generateQuestions(int count = 5);
    Q_INVOKABLE void clearQuestions();

signals:
    void questionsGenerated(const QStringList& questions);

private slots:
    // Reactive cleanup
    void onActiveSessionChanged();

    // Engine result handler
    void onLLMTextGenerated(const QString& text, int requestId);

private:
    // Injected Dependencies
    AiService* m_ai;
    SessionService* m_sessions;

    // UI Model
    QStringListModel* m_questionsList;

    // Request tracking
    int m_pendingRequestId;
    int m_nextRequestId;

    // Unique ID for Question generation requests (distinct from Chat IDs)
    static constexpr int QUESTION_REQ_BASE = 9000;
};
