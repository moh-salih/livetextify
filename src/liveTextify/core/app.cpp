#include "liveTextify/core/app.h"
#include "liveTextify/core/resourcemanager.h"
#include "liveTextify/core/resourcemonitor.h"
#include "liveTextify/core/Logger.h"
#include "liveTextify/platform/PlatformManager.h"
#include "liveTextify/modules/database/databasemanager.h"
#include "liveTextify/modules/session/sessionmanager.h"
#include "liveTextify/modules/session/sessionsettings.h"
#include "liveTextify/modules/models/modelmanager.h"
#include "liveTextify/modules/services/sessionservice.h"

#include <QtWhisper/Types.h>
#include <QtWhisper/Session.h>
#include <QtLlama/Types.h>
#include <QtLlama/Session.h>
#include <QtLlama/Embedder.h>

#include <QQmlContext>
#include <QIcon>
#include <QQuickStyle>
#include <QStandardPaths>
#include <QSqlError>

static QString getLocalPathFor(const QString& relativeFilePath) {
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
    .append("/" + relativeFilePath);
}

bool createTables(const QSqlDatabase& db) {
    QSqlQuery query(db);

    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS sessions ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "title TEXT NOT NULL,"
            "created_at INTEGER NOT NULL,"
            "llm_model_path TEXT,"
            "stt_model_path TEXT,"
            "embedding_model_path TEXT,"
            "system_prompt TEXT,"
            "max_context_length INTEGER DEFAULT 8000,"
            "enable_rag INTEGER DEFAULT 0,"
            "transcription_language TEXT DEFAULT 'en',"
            "auto_detect_language INTEGER DEFAULT 0"
            ")"
            )) {
        Logger::error(QString("Failed to create sessions table: %1").arg(query.lastError().text()));
        return false;
    }

    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS messages ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "session_id INTEGER NOT NULL,"
            "role TEXT NOT NULL,"
            "content TEXT NOT NULL,"
            "timestamp INTEGER NOT NULL,"
            "message_index INTEGER NOT NULL,"
            "FOREIGN KEY (session_id) REFERENCES sessions(id) ON DELETE CASCADE"
            ")"
            )) {
        Logger::error(QString("Failed to create messages table: %1").arg(query.lastError().text()));
        return false;
    }

    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS transcriptions ("
            "session_id INTEGER PRIMARY KEY,"
            "text TEXT NOT NULL,"
            "updated_at INTEGER NOT NULL,"
            "FOREIGN KEY (session_id) REFERENCES sessions(id) ON DELETE CASCADE"
            ")"
            )) {
        Logger::error(QString("Failed to create transcriptions table: %1").arg(query.lastError().text()));
        return false;
    }

    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS chunks ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "session_id INTEGER NOT NULL,"
            "text TEXT NOT NULL,"
            "chunk_index INTEGER NOT NULL,"
            "vector BLOB,"
            "FOREIGN KEY (session_id) REFERENCES sessions(id) ON DELETE CASCADE"
            ")"
            )) {
        Logger::error(QString("Failed to create chunks table: %1").arg(query.lastError().text()));
        return false;
    }

    query.exec("CREATE INDEX IF NOT EXISTS idx_messages_session ON messages(session_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_chunks_session ON chunks(session_id)");

    return true;
}

App::App(int& argc, char* argv[], QObject* parent)
    : QObject(parent)
    , mApp(argc, argv)
{
    setupApplicationInfo();
    Logger::initialize(getLocalPathFor("logs/liveTextify.log"), "LiveTextify");
}

App::~App() {
    Logger::info("=== LiveTextify Shutting Down ===");
}

bool App::init() {
    bool success = false;
    try{
        setupControllers();
        setupConnections();
        loadInitialConfigs();

        if (mResourceManager)
            mResourceManager->loadResources();

        QString appDir = QCoreApplication::applicationDirPath();
        mEngine.addImportPath(appDir + "/qml");
        mEngine.addImportPath("qrc:/qt/qml");

        mEngine.loadFromModule("liveTextify", "Main");
        success = !mEngine.rootObjects().isEmpty();
        success ? Logger::info("Application initialized successfully.") : Logger::critical("Failed to load QML engine.");
    }catch(const std::exception& e){
        Logger::error(QString("Exception: ").append(e.what()));
    }catch(...){
        Logger::error("Unknown exception has occurred!");
    }

    return success;
}

int App::exec() {
    return init() ? mApp.exec() : -1;
}

void App::setupApplicationInfo() {
    mApp.setOrganizationName("SApps");
    mApp.setOrganizationDomain("com.salih.livetextify");
    mApp.setApplicationName("liveTextify");
    mApp.setApplicationDisplayName("LiveTextify: Voice to Text to Insight");
    mApp.setApplicationVersion("1.0.0");
    mApp.setWindowIcon(QIcon(":/icons/app.png"));
}

void App::setupControllers() {
    bool dbInitialized = DatabaseManager::instance()->initialize(getLocalPathFor("data/liveTextify.db"));
    if (dbInitialized)
        DatabaseManager::instance()->createTables(createTables);

    // Register enum namespaces for QML
    qmlRegisterUncreatableMetaObject(QtWhisper::staticMetaObject, "QtWhisper", 1, 0, "QtWhisper", "Enum namespace");
    qmlRegisterUncreatableMetaObject(QtLlama::staticMetaObject,   "QtLlama",   1, 0, "QtLlama",   "Enum namespace");

    // Register non-instantiable types for QML type safety
    qmlRegisterUncreatableType<QtWhisper::Session>("LiveTextify", 1, 0, "WhisperSession", "Cannot instantiate");
    qmlRegisterUncreatableType<QtLlama::Session>  ("LiveTextify", 1, 0, "LlamaSession",   "Cannot instantiate");
    qmlRegisterUncreatableType<QtLlama::Embedder> ("LiveTextify", 1, 0, "LlamaEmbedder",  "Cannot instantiate");

    mResourceMonitor = new ResourceMonitor(this);
    mResourceMonitor->start(2000);
    mResourceManager = new ResourceManager(this);
    mPlatformManager = &PlatformManager::instance();
    mSessionManager  = new SessionManager(this);
    mModelManager    = new ModelManager(this);

    // ModelManager notifies SessionSettings directly when the user picks a model
    connect(mModelManager, &ModelManager::defaultModelChanged, mSessionManager->sessionService()->settings(), &SessionSettings::setModelPath);

    auto ctx = mEngine.rootContext();
    ctx->setContextProperty("PlatformManager", mPlatformManager);
    ctx->setContextProperty("ResourceManager", mResourceManager);
    ctx->setContextProperty("ResourceMonitor", mResourceMonitor);
    ctx->setContextProperty("SessionManager",  mSessionManager);
    ctx->setContextProperty("ModelManager",    mModelManager);

    if (mPlatformManager->isAndroid()) {
        auto* android = mPlatformManager->android();
        if (android)
            ctx->setContextProperty("Android", android);
    }
}

void App::setupConnections() {
    connect(&mEngine, &QQmlApplicationEngine::objectCreated,
            this, [](QObject* object, const QUrl& url) {
                if (!object)
                    Logger::error("Failed to create object from: " + url.toString());
            });

    connect(&mEngine, &QQmlApplicationEngine::quit, &mApp, &QGuiApplication::quit);
}

void App::loadInitialConfigs() {
    QQuickStyle::setStyle("Material");

    SessionSettings* settings = mSessionManager->sessionService()->settings();

    mModelManager->stt()->loadConfig(":/data/whisper_models.json");
    mModelManager->llm()->loadConfig(":/data/llama_models.json");
    mModelManager->emb()->loadConfig(":/data/embedding_models.json");

    // Sync ModelManager's selected path from what SessionSettings persisted
    mModelManager->stt()->syncSelectedPath(settings->toConfig().stt.modelPath);
    mModelManager->llm()->syncSelectedPath(settings->toConfig().llm.modelPath);
    mModelManager->emb()->syncSelectedPath(settings->toConfig().emb.modelPath);
}
