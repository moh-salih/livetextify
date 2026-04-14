#include "liveTextify/modules/models/modelmanager.h"
#include "liveTextify/core/Logger.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QStandardPaths>

ModelManager::ModelManager(const QString& settingsKey, QObject *parent)
    : QObject(parent)
    , m_viewModel(new ModelListModel(this)) // Create the view model internally
    , m_settingsKey(settingsKey)
{
    m_baseDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/models";
    QDir().mkpath(m_baseDir);


    cleanDownloadLeftovers();
}

void ModelManager::loadDefaultModel(){
    QSettings settings;
    m_defaultPath = settings.value(m_settingsKey).toString();
    Logger::info(QString("ModelManager [%1]: Default path from settings: '%2'").arg(m_settingsKey).arg(m_defaultPath));
    validateDefaultModel();
    emit defaultModelPathChanged(m_defaultPath);
}

bool ModelManager::loadConfig(const QString &jsonPath) {
    Logger::info(QString("ModelManager: Loading config from %1").arg(jsonPath));

    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly)){
        Logger::error(QString("Failed to open config: %1").arg(jsonPath));
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QVector<ModelItem> items;
    for (const QJsonValue &val : doc.array()) {
        QJsonObject obj = val.toObject();
        items.append({ obj["fileName"].toString(), obj["url"].toString(), obj["size"].toString() });
    }

    m_viewModel->populate(items);
    syncDownloadedState();
    validateDefaultModel();

    if (!m_defaultPath.isEmpty()) {
        int idx = m_viewModel->findIndexByPath(m_defaultPath, m_baseDir);
        m_viewModel->setExclusiveDefault(idx);
    }

    return true;
}

void ModelManager::downloadModel(int index) {
    if (index < 0 || index >= m_viewModel->rowCount()) return;
    const auto& item = m_viewModel->getItem(index);
    QString path = getStoragePath(item.url);

    FileDownloader *dl = new FileDownloader(QUrl(item.url), path, &m_nam, this);
    m_activeDownloads.insert(dl, {index});
    m_viewModel->setDownloadingStatus(index, true);
    m_viewModel->updateProgress(index, 0.0f);


    connect(dl, &FileDownloader::progressChanged, this, [this, dl](float p){
        m_viewModel->updateProgress(m_activeDownloads[dl].index, p);
    });

    connect(dl, &FileDownloader::finished, this, [this, dl](bool ok, QString err){
        auto task = m_activeDownloads.take(dl);
        m_viewModel->setDownloadingStatus(task.index, false);
        if (ok) {
            m_viewModel->setDownloadedStatus(task.index, true);
            m_viewModel->updateProgress(task.index, 1.0f);
            emit downloadFinished(true);
        } else {
            emit errorOccurred(err);
        }
        dl->deleteLater();
        emit isDownloadingChanged();
    });

    dl->start();
    emit isDownloadingChanged();
}

void ModelManager::selectDefaultModel(int index) {
    if (index < 0 || index >= m_viewModel->rowCount()) return;
    const auto& item = m_viewModel->getItem(index);
    if (!item.downloaded) return;

    m_defaultPath = getStoragePath(item.url);

    QSettings settings;
    settings.setValue(m_settingsKey, m_defaultPath);
    settings.sync();

    updateDefaultInViewModel();
    emit defaultModelPathChanged(m_defaultPath);
}

void ModelManager::syncDownloadedState() {
    for (int i = 0; i < m_viewModel->rowCount(); ++i) {
        bool exists = QFile::exists(getStoragePath(m_viewModel->getItem(i).url));
        m_viewModel->setDownloadedStatus(i, exists);
        m_viewModel->updateProgress(i, exists ? 1.0f : 0.0f);
    }
}

QString ModelManager::getStoragePath(const QString& url) const {
    return m_baseDir + "/" + QUrl(url).fileName();
}

void ModelManager::cancelDownload(const QString& url) {
    for (auto it = m_activeDownloads.begin(); it != m_activeDownloads.end(); ++it) {
        if (it.key()->url().toString() == url) {
            it.key()->cancel();
            m_viewModel->setDownloadedStatus(it.value().index, false);
            m_viewModel->setDownloadingStatus(it.value().index, false);
            m_viewModel->updateProgress(it.value().index, 0);
            return;
        }
    }
}

void ModelManager::deleteModel(const QString& url) {
    QString path = getStoragePath(url);
    if (QFile::exists(path) && QFile::remove(path)) {
        syncDownloadedState();
        if (path == m_defaultPath) {
            clearDefaultPersistence();
        }
    }
}

void ModelManager::cleanDownloadLeftovers(){
    QDir dir(m_baseDir);
    QStringList partialFiles = dir.entryList({"*.part"});
    foreach(const QString& filename , partialFiles) {
        dir.remove(filename);
    }
}

void ModelManager::updateDefaultInViewModel() {
    if (m_defaultPath.isEmpty()) return;
    int idx = m_viewModel->findIndexByPath(m_defaultPath, m_baseDir);
    if (idx != -1) {
        m_viewModel->setExclusiveDefault(idx);
    }
}

void ModelManager::validateDefaultModel() {
    if (m_defaultPath.isEmpty()) return;
    if (!QFile::exists(m_defaultPath)) {
        clearDefaultPersistence();
    } else {
        updateDefaultInViewModel();
    }
}

void ModelManager::clearDefaultPersistence() {
    m_defaultPath.clear();
    QSettings settings;
    settings.remove(m_settingsKey);
    settings.sync();
    m_viewModel->setExclusiveDefault(-1);
    emit defaultModelPathChanged("");
}
