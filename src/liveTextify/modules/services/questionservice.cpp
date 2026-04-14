#include "questionservice.h"
#include "liveTextify/modules/session/session.h"
#include "liveTextify/modules/engines/llm/llmsession.h"

#include "liveTextify/core/Logger.h"
#include <QRegularExpression>


QuestionService::QuestionService(
    AiService* aiService,
    SessionService* sessionService,
    QObject* parent)
    : QObject(parent)
    , m_ai(aiService)
    , m_sessions(sessionService)
    , m_questionsList(new QStringListModel(this))
    , m_pendingRequestId(-1)
    , m_nextRequestId(QUESTION_REQ_BASE)
{
    // 1. ENGINE INTERACTION: Listen to LLM results from AiService
    connect(m_ai->llmSession(), &LlmSession::textGenerated, this, &QuestionService::onLLMTextGenerated);

    // 2. REACTIVE CLEANUP: Clear questions when the user switches sessions
    connect(m_sessions, &SessionService::activeSessionChanged, this, &QuestionService::onActiveSessionChanged);
}

void QuestionService::onActiveSessionChanged() {
    clearQuestions();
    m_pendingRequestId = -1;
    Logger::debug("QuestionService: Questions cleared due to session change.");
}

void QuestionService::generateQuestions(int count) {
    Session* active = m_sessions->activeSession();

    if (!active || active->transcription().isEmpty()) {
        Logger::warning("QuestionService: Cannot generate questions (No active session or empty transcription)");

        return;
    }

    Logger::info(QString("QuestionService: Generating %1 questions for session %2").arg(count).arg(active->sessionID()));

    // Build the prompt using the active session's data
    QString prompt = QString(
                         "Based on the following transcription, generate %1 short, insightful questions "
                         "that a listener might want to ask. Return only the questions, one per line.\n\n"
                         "Transcription:\n%2"
                         ).arg(count).arg(active->transcription());

    m_pendingRequestId = m_nextRequestId++;

    // Send to LLM hardware via injected AI Service
    m_ai->llmSession()->generate(prompt, m_pendingRequestId);
}

void QuestionService::clearQuestions() {
    m_questionsList->setStringList(QStringList());
}

void QuestionService::onLLMTextGenerated(const QString& text, int requestId) {
    // Only process if this is the response to our specific question request
    if (requestId != m_pendingRequestId) return;

    QStringList questions;
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);

    for (const QString& line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty()) continue;

        // Strip Markdown/Numbers: "1. Question?" -> "Question?"
        QString cleaned = trimmed;
        cleaned.remove(QRegularExpression("^\\d+[.)]\\s*"));
        cleaned.remove(QRegularExpression("^[-•*]\\s*"));

        if (!cleaned.isEmpty()) {
            questions.append(cleaned);
        }
    }

    if (!questions.isEmpty()) {
        m_questionsList->setStringList(questions);
        emit questionsGenerated(questions);
        Logger::info(QString("QuestionService: Parsed %1 questions successfully.").arg(questions.size()));

    }
}
