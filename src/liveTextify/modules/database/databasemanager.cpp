#include "liveTextify/modules/database/databasemanager.h"
#include "liveTextify/core/Logger.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>


DatabaseManager* DatabaseManager::mInstance = nullptr;

DatabaseManager::DatabaseManager(QObject *parent): QObject(parent), mInitialized(false){

}

DatabaseManager::~DatabaseManager()
{
    close();
}

DatabaseManager* DatabaseManager::instance(){
    if (!mInstance) {
        mInstance = new DatabaseManager();
    }
    return mInstance;
}

bool DatabaseManager::initialize(const QString& dbPath){
    QMutexLocker locker(&mMutex);

    if (mInitialized) return true;

    QDir dir;
    dir.mkpath(QFileInfo(dbPath).absolutePath());

    mDB = QSqlDatabase::addDatabase("QSQLITE");
    mDB.setDatabaseName(dbPath);

    if (!mDB.open()) {
        mLastError = "Failed to open database: " + mDB.lastError().text();
        Logger::critical(mLastError);
        return false;
    }

 
    mInitialized = true;
    Logger::info("Database initialized: " + dbPath);
    return true;
}

void DatabaseManager::close()
{
    QMutexLocker locker(&mMutex);

    if (mDB.isOpen()) {
        mDB.close();
    }
    mInitialized = false;
}



bool DatabaseManager::createTables(TableCreationFunc schemaCallback){
    if(schemaCallback) {
        if (!schemaCallback(mDB)) {
            mLastError = "Custom schema creation failed";
            return false;
        }
        return true;
    }
    return false;
}

bool DatabaseManager::beginTransaction()
{
    if (!mInitialized) return false;
    return mDB.transaction();
}

bool DatabaseManager::commitTransaction()
{
    if (!mInitialized) return false;
    return mDB.commit();
}

bool DatabaseManager::rollbackTransaction()
{
    if (!mInitialized) return false;
    return mDB.rollback();
}

int DatabaseManager::lastInsertId()
{
    QSqlQuery query(mDB);
    if (query.exec("SELECT last_insert_rowid()") && query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}
