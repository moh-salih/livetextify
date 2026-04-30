#include "liveTextify/core/app.h"
#include "liveTextify/modules/settings/settingsmanager.h"
#include "liveTextify/core/resourcemanager.h"
#include "liveTextify/platform/PlatformManager.h"
#include "liveTextify/modules/database/databasemanager.h"
#include "liveTextify/core/Logger.h"

#include "liveTextify/modules/session/sessionmanager.h"
#include "liveTextify/modules/models/modelmanager.h"

#include <QtWhisper/Types.h>
#include <QtWhisper/Session.h>
#include <QtLlama/Types.h>
#include <QtLlama/Session.h>
#include <QtLlama/Embedder.h>
#include <QtAudioCapture/Types.h>

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
 


  // Register Namespaces for Enums
    qmlRegisterUncreatableMetaObject(QtWhisper::staticMetaObject, "QtWhisper", 1, 0, "QtWhisper", "Enum namespace");
    qmlRegisterUncreatableMetaObject(QtLlama::staticMetaObject, "QtLlama", 1, 0, "QtLlama", "Enum namespace");
    // qmlRegisterUncreatableMetaObject(QtAudioCapture::staticMetaObject, "QtAudioCapture", 1, 0, "QtAudioCapture", "Enum namespace");

    // Register Type bindings
    qmlRegisterUncreatableType<QtWhisper::Session>("LiveTextify", 1, 0, "WhisperSession", "Cannot instantiate");
    qmlRegisterUncreatableType<QtLlama::Session>("LiveTextify", 1, 0, "LlamaSession", "Cannot instantiate");
    qmlRegisterUncreatableType<QtLlama::Embedder>("LiveTextify", 1, 0, "LlamaEmbedder", "Cannot instantiate");
    


    mSettingsManager = new SettingsManager(this);
    mResourceManager = new ResourceManager(this);
    mPlatformManager = &PlatformManager::instance();
    mSessionManager  = new SessionManager(mSettingsManager, this);
    mModelManager    = new ModelManager(this);


    connect(mModelManager,    &ModelManager::defaultModelChanged, mSettingsManager, &SettingsManager::onDefaultModelChanged);

    mSettingsManager->reload();


    auto ctx = mEngine.rootContext();
    ctx->setContextProperty("PlatformManager", mPlatformManager);
    ctx->setContextProperty("SettingsManager", mSettingsManager);
    ctx->setContextProperty("ResourceManager", mResourceManager);
    ctx->setContextProperty("SessionManager",  mSessionManager);
    ctx->setContextProperty("ModelManager",    mModelManager);


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

    mModelManager->stt()->loadConfig(":/data/whisper_models.json");
    mModelManager->llm()->loadConfig(":/data/llama_models.json");
    mModelManager->emb()->loadConfig(":/data/embedding_models.json");

    mModelManager->stt()->syncSelectedPath(mSettingsManager->sttConfigManager()->modelPath());
    mModelManager->llm()->syncSelectedPath(mSettingsManager->llmConfigManager()->modelPath());
    mModelManager->emb()->syncSelectedPath(mSettingsManager->embConfigManager()->modelPath());
}
