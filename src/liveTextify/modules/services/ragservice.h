#pragma once
#include <QObject>
#include <memory>
#include <QtRag/RagTypes.h>
#include <QtRag/RagConfig.h>
#include <QtLlama/Types.h>

class Session;
struct SessionConfig;
class DatabaseService;

namespace QtLlama  { class Embedder; }
namespace QtRag    { class RagEngine; class IRagEmbedder; class IRagStorage; }

class RagService : public QObject {
    Q_OBJECT
public:
    explicit RagService(DatabaseService* database, QObject* parent = nullptr);
    ~RagService() override;

    bool isReady() const;
    QtLlama::Status embedderStatus() const;
public slots:
    void onActiveSessionChanged(Session* activeSession);
    void onConfigChanged(const SessionConfig& config);
    void onTranscriptionUpdated(const QString& fullText, int sessionId);
    void requestContext(const QString& text, int sessionId);
    void reloadModels();

signals:
    void contextReady(const QString& context, int sessionId);
    void embedderStatusChanged(QtLlama::Status status);
    void reloadRequired();
    void errorOccurred(QtRag::Error error);
    void embedderErrorOccurred(QtLlama::Error error);

private:
    void loadModels(Session* activeSession);
    void unloadModels();
    void rebuildIndex();

    Session*         m_activeSession      = nullptr;
    DatabaseService* m_database;
    int              m_lastProcessedLength = 0;

    QtLlama::Embedder*                   m_embedder;
    std::unique_ptr<QtRag::IRagEmbedder> m_ragEmbedderAdapter;
    std::unique_ptr<QtRag::IRagStorage>  m_ragStorageAdapter;
    std::unique_ptr<QtRag::RagEngine>    m_ragEngine;
};
