#pragma once
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMutex>
#include <QString>

class DatabaseManager : public QObject{
    Q_OBJECT
public:
    using TableCreationFunc = std::function<bool(QSqlDatabase&)>;
    static DatabaseManager* instance();

    bool initialize(const QString& dbPath);
    void close();

    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

    QSqlDatabase database() const { return mDB; }

    // Utility methods
    int lastInsertId();
    QString errorString() const { return mLastError; }

    bool createTables(TableCreationFunc schemaCallback = nullptr);
private:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();


    static DatabaseManager* mInstance;
    QSqlDatabase mDB;
    QMutex mMutex;
    QString mLastError;
    bool mInitialized;
};


