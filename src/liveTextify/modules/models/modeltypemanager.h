#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include "liveTextify/modules/models/modellistmodel.h"
#include "liveTextify/modules/models/filedownloader.h"

class ModelTypeManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(ModelListModel* models     READ models     CONSTANT)
    Q_PROPERTY(bool isDownloading         READ isDownloading NOTIFY isDownloadingChanged)
    Q_PROPERTY(QString selectedPath       READ selectedPath  NOTIFY selectedPathChanged)

public:
    explicit ModelTypeManager(const QString& storageSubDir, QObject* parent = nullptr);

    Q_INVOKABLE bool    loadConfig(const QString& jsonPath);
    Q_INVOKABLE void    downloadModel(int index);
    Q_INVOKABLE void    cancelDownload(const QString& url);
    Q_INVOKABLE void    deleteModel(const QString& url);
    Q_INVOKABLE void    selectDefault(int index);
    Q_INVOKABLE void    syncSelectedPath(const QString& path);
    Q_INVOKABLE QString storagePath(const QString& url) const;
    Q_INVOKABLE bool    isDownloaded(const QString& url) const;

    ModelListModel* models()      const { return mModel; }
    bool            isDownloading() const { return !mActiveDownloads.isEmpty(); }
    QString         selectedPath()  const { return mSelectedPath; }

signals:
    void isDownloadingChanged();
    void selectedPathChanged(const QString& path);
    void downloadFinished(bool success);
    void errorOccurred(const QString& msg);

private:
    void syncDownloadedState();
    void cleanLeftovers();

    QString          mBaseDir;
    ModelListModel*  mModel;
    QString          mSelectedPath;
    QNetworkAccessManager mNam;

    struct Task { int index; };
    QHash<FileDownloader*, Task> mActiveDownloads;
};


class SttModelManager : public ModelTypeManager {
    Q_OBJECT
public:
    explicit SttModelManager(QObject* parent = nullptr): ModelTypeManager("stt", parent) {}
};


class LlmModelManager : public ModelTypeManager {
    Q_OBJECT
public:
    explicit LlmModelManager(QObject* parent = nullptr): ModelTypeManager("llm", parent) {}
};


class EmbModelManager : public ModelTypeManager {
    Q_OBJECT
public:
    explicit EmbModelManager(QObject* parent = nullptr): ModelTypeManager("emb", parent) {}
};


