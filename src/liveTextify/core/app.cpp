#include "liveTextify/core/app.h"
#include "liveTextify/modules/settings/settingsmanager.h"
#include "liveTextify/core/resourcemanager.h"
#include "liveTextify/platform/PlatformManager.h"
#include "liveTextify/modules/database/databasemanager.h"
#include "liveTextify/core/Logger.h"

#include "liveTextify/modules/session/sessionmanager.h"
#include "liveTextify/modules/models/modelmanager.h"

#include "liveTextify/modules/engines/stt/sttsession.h"
#include "liveTextify/modules/engines/llm/llmsession.h"
#include "liveTextify/modules/engines/emb/embsession.h"

#include <QQmlContext>
#include <QIcon>
#include <QQuickStyle>
#include <QStandardPaths>
#include <QSqlError>

static QString getLocalPathFor(const QString& relativeFilePath){
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation).append("/"+ relativeFilePath);
}


bool createTables(const QSqlDatabase& db){
    QSqlQuery query(db);

    // Sessions table (Audio fields removed)
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
            )
    )
    {
        Logger::error(QString("Failed to create sessions table: %").arg(query.lastError().text()));
        return false;
    }

    // Messages table
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

        Logger::error(QString("Failed to create messages table: %").arg(query.lastError().text()));
        return false;
    }

    // Transcriptions table
    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS transcriptions ("
            "session_id INTEGER PRIMARY KEY,"
            "text TEXT NOT NULL,"
            "updated_at INTEGER NOT NULL,"
            "FOREIGN KEY (session_id) REFERENCES sessions(id) ON DELETE CASCADE"
            ")"
            )) {
        Logger::error(QString("Failed to create transcriptions table: %").arg(query.lastError().text()));
        return false;
    }

    // Chunks table
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

        Logger::error(QString("Failed to create chunks table: %").arg(query.lastError().text()));
        return false;
    }

    query.exec("CREATE INDEX IF NOT EXISTS idx_messages_session ON messages(session_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_chunks_session ON chunks(session_id)");

    return true;
}


App::App(int& argc, char* argv[], QObject *parent): QObject(parent), mApp(argc, argv){
    setupApplicationInfo();
    Logger::initialize(getLocalPathFor("logs/liveTextify.log"), "LiveTextify");
}

App::~App(){
    Logger::info("=== LiveTextify Shutting Down ===");
}

bool App::init(){
    setupControllers();
    setupConnections();
    loadInitialConfigs();

    if (mResourceManager) {
        mResourceManager->loadResources();
    }


    QString appDir = QCoreApplication::applicationDirPath();
    mEngine.addImportPath(appDir + "/qml");
    mEngine.addImportPath("qrc:/qt/qml");



    mEngine.loadFromModule("liveTextify", "Main");
    bool success = !mEngine.rootObjects().isEmpty();
    success ? Logger::info("Application initialized successfully.") : Logger::critical("Failed to load QML engine.");
    return success;
}

int App::exec(){
    return init() ? mApp.exec() : -1;
}

void App::setupApplicationInfo()
{
    mApp.setOrganizationName("SApps");
    mApp.setOrganizationDomain("com.salih.livetextify");
    mApp.setApplicationName("liveTextify");
    mApp.setApplicationDisplayName("LiveTextify: Voice to Text to Insight");
    mApp.setApplicationVersion("1.0.0");
    mApp.setWindowIcon(QIcon(":/icons/app.png"));
}

void App::setupControllers(){
    bool dbInitialized = DatabaseManager::instance()->initialize(getLocalPathFor("data/liveTextify.db"));
    if(dbInitialized){
        DatabaseManager::instance()->createTables(createTables);
    }
    qmlRegisterUncreatableMetaObject( LiveTextify::staticMetaObject, "LiveTextify", 1, 0, "LiveTextify", "Accessing the LiveTextify namespace directly is not allowed.");

    qmlRegisterUncreatableType<SttSession>("LiveTextify", 1, 0, "SttSession", "SttSession is an enum type, cannot be instantiated");
    qmlRegisterUncreatableType<LlmSession>("LiveTextify", 1, 0, "LlmSession", "LlmSession is an enum type, cannot be instantiated");
    qmlRegisterUncreatableType<EmbSession>("LiveTextify", 1, 0, "EmbSession", "EmbSession is an enum type, cannot be instantiated");


    mSettingsManager = new SettingsManager(this);
    mResourceManager = new ResourceManager(this);
    mPlatformManager = &PlatformManager::instance();
    mSessionManager  = new SessionManager(mSettingsManager, this);

    mWhisperManager         = new ModelManager("defaultSttPath", this);
    mLLamaManager           = new ModelManager("defaultLlmPath", this);
    mEmbeddingManager       = new ModelManager("defaultEmbeddingModelPath", this);

    auto ctx = mEngine.rootContext();
    ctx->setContextProperty("PlatformManager", mPlatformManager);
    ctx->setContextProperty("SettingsManager", mSettingsManager);
    ctx->setContextProperty("ResourceManager", mResourceManager);

    ctx->setContextProperty("SessionManager",       mSessionManager);
    ctx->setContextProperty("LlamaManager",         mLLamaManager);
    ctx->setContextProperty("WhisperManager",       mWhisperManager);
    ctx->setContextProperty("EmbeddingManager",     mEmbeddingManager);


    if (mPlatformManager->isAndroid()) {
        auto* android = mPlatformManager->android();
        if (android) {
            ctx->setContextProperty("Android", android);            
        }
    }
}

void App::setupConnections()
{
    connect(&mEngine, &QQmlApplicationEngine::objectCreated,
            this, [](QObject* object, const QUrl& url) {
                if (!object) {
                    Logger::error("Failed to create object from: " + url.toString());
                }
            });
            
    connect(&mEngine, &QQmlApplicationEngine::quit, &mApp, &QGuiApplication::quit);
}

void App::loadInitialConfigs(){
    QQuickStyle::setStyle("Material");

    mWhisperManager->loadDefaultModel();
    mWhisperManager->loadConfig(":/data/whisper_models.json");

    mLLamaManager->loadDefaultModel();
    mLLamaManager->loadConfig(":/data/llama_models.json");

    mEmbeddingManager->loadDefaultModel();
    mEmbeddingManager->loadConfig(":/data/embedding_models.json");
}
