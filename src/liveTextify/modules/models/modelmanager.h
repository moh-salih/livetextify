#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QHash>
#include "liveTextify/modules/models/modellistmodel.h"
#include "liveTextify/modules/models/filedownloader.h"

// A generic manager for any type of AI model (STT, LLM, etc.)
// Instantiated multiple times for different model types.
class ModelManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(ModelListModel* listModel READ listModel CONSTANT)
    Q_PROPERTY(bool isDownloading READ isDownloading NOTIFY isDownloadingChanged)
    Q_PROPERTY(QString defaultModelPath READ defaultModelPath NOTIFY defaultModelPathChanged)

public:
    explicit ModelManager(const QString& settingsKey, QObject *parent = nullptr);

    ModelListModel* listModel() const { return m_viewModel; }
    bool isDownloading() const { return !m_activeDownloads.isEmpty(); }
    QString defaultModelPath() const { return m_defaultPath; }

    Q_INVOKABLE void loadDefaultModel();
    Q_INVOKABLE bool loadConfig(const QString &jsonPath);
    Q_INVOKABLE void downloadModel(int index);
    Q_INVOKABLE void cancelDownload(const QString &url);
    Q_INVOKABLE void selectDefaultModel(int index);
    Q_INVOKABLE void deleteModel(const QString &url);

signals:
    void isDownloadingChanged();
    void defaultModelPathChanged(const QString &path);
    void errorOccurred(const QString &msg);
    void downloadFinished(bool success);

private:
    void syncDownloadedState();
    QString getStoragePath(const QString& url) const;
    void cleanDownloadLeftovers();
    void validateDefaultModel();
    void clearDefaultPersistence();
    void updateDefaultInViewModel();

    ModelListModel* m_viewModel;
    QNetworkAccessManager m_nam;
    QString m_settingsKey;
    QString m_defaultPath;
    QString m_baseDir;

    struct Task { int index; };
    QHash<FileDownloader*, Task> m_activeDownloads;
};
