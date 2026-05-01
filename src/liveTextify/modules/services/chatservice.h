#pragma once
#include <QObject>
#include <memory>
#include <QtLlama/Types.h>
#include <QtRag/RagTypes.h>
#include "liveTextify/modules/session/session.h"
#include "liveTextify/core/AppError.h"

namespace QtLlama { class Session; class Embedder; }
namespace QtRag   { class RagEngine; class IRagEmbedder; class IRagStorage; }

class DatabaseService;

class ChatService : public QObject {
    Q_OBJECT
public:
    explicit ChatService(DatabaseService* database, QObject* parent = nullptr);
    ~ChatService() override;

    QtLlama::Status llamaStatus()    const;
    QtLlama::Status embedderStatus() const;

public slots:
    void onActiveSessionChanged(Session* activeSession);
    void onUserMessageReady(const QString& text, int sessionId);
    void onTranscriptionUpdated(const QString& fullText, int sessionId);
    void stopGeneration();
    void onActiveSessionConfigChanged(const SessionConfig& config);

signals:
    void llamaStatusChanged(QtLlama::Status status);
    void embedderStatusChanged(QtLlama::Status status);
    void isLlamaGeneratingChanged(bool generating);
    void assistantMessageChunk(const QString& text, int sessionId);
    void assistantMessageComplete(int sessionId);

    // Separate signals per error source so SessionManager
    // can map each to the correct AppError converter
    void llamaErrorOccurred(QtLlama::Error error);
    void embedderErrorOccurred(QtLlama::Error error);
    void ragErrorOccurred(QtRag::Error error);

private slots:
    void onLLMTextGenerated(const QString& text, int sessionId);
    void onLLMStateChanged(bool generating);
    void onContextReady(const QString& context, int sessionId);

private:
    void generateLlmPrompt(const QString& userText, int sessionId);
    void loadModels(Session* activeSession);
    void unloadModels();
    void updateRagConfig();

    Session*         m_activeSession = nullptr;
    DatabaseService* m_database;

    QtLlama::Session*  m_llama;
    QtLlama::Embedder* m_embedder;

    std::unique_ptr<QtRag::IRagEmbedder> m_ragEmbedderAdapter;
    std::unique_ptr<QtRag::IRagStorage>  m_ragStorageAdapter;
    std::unique_ptr<QtRag::RagEngine>    m_ragEngine;

    bool    m_waitingForContext          = false;
    QString m_pendingUserMessage;
    int     m_pendingSessionId           = -1;
    int     m_currentGeneratingSessionId = -1;
    int     m_lastProcessedLength        = 0;
};
