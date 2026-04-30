#include "liveTextify/modules/models/modeltypemanager.h"
#include "liveTextify/core/Logger.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <QStandardPaths>

ModelTypeManager::ModelTypeManager(const QString& storageSubDir, QObject* parent)
    : QObject(parent)
    , mBaseDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
               + "/models/" + storageSubDir)
    , mModel(new ModelListModel(this))
{
    QDir().mkpath(mBaseDir);
    cleanLeftovers();
}

bool ModelTypeManager::loadConfig(const QString& jsonPath) {
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly)) {
        Logger::error(QString("ModelTypeManager: failed to open config: %1").arg(jsonPath));
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QVector<ModelItem> items;
    for (const QJsonValue& val : doc.array()) {
        QJsonObject obj = val.toObject();
        items.append({ obj["fileName"].toString(), obj["url"].toString(), obj["size"].toString() });
    }

    mModel->populate(items);
    syncDownloadedState();

    if (!mSelectedPath.isEmpty())
        syncSelectedPath(mSelectedPath);

    Logger::info(QString("ModelTypeManager: loaded %1 entries from %2").arg(items.size()).arg(jsonPath));
    return true;
}

void ModelTypeManager::selectDefault(int index) {
    if (index < 0 || index >= mModel->rowCount()) return;

    const ModelItem& item = mModel->getItem(index);
    if (!item.downloaded) return;

    const QString path = storagePath(item.url);
    mSelectedPath = path;
    mModel->setExclusiveDefault(index);

    emit selectedPathChanged(path);
}

void ModelTypeManager::syncSelectedPath(const QString& path) {
    if (path.isEmpty()) {
        mSelectedPath.clear();
        mModel->setExclusiveDefault(-1);
        return;
    }

    const int idx = mModel->findIndexByPath(path, mBaseDir);

    // Ensure the model exists and is fully downloaded before setting it as default
    if (idx >= 0 && mModel->getItem(idx).downloaded) {
        mSelectedPath = path;
        mModel->setExclusiveDefault(idx);
    } else {
        // Model file is missing or not downloaded -> unset it
        mSelectedPath.clear();
        mModel->setExclusiveDefault(-1);
        emit selectedPathChanged(QString()); // Notify settings to wipe the invalid path
    }
}

void ModelTypeManager::downloadModel(int index) {
    if (index < 0 || index >= mModel->rowCount()) return;

    const ModelItem& item = mModel->getItem(index);
    const QString path = storagePath(item.url);

    auto* dl = new FileDownloader(QUrl(item.url), path, &mNam, this);
    mActiveDownloads.insert(dl, { index });

    mModel->setDownloadingStatus(index, true);
    mModel->updateProgress(index, 0.0f);

    connect(dl, &FileDownloader::progressChanged, this, [this, dl](float p) {
        const auto& task = mActiveDownloads[dl];
        mModel->updateProgress(task.index, p);
    });

    connect(dl, &FileDownloader::finished, this, [this, dl](bool ok, const QString& err) {
        const auto task = mActiveDownloads.take(dl);

        mModel->setDownloadingStatus(task.index, false);

        if (ok) {
            mModel->setDownloadedStatus(task.index, true);
            mModel->updateProgress(task.index, 1.0f);
            emit downloadFinished(true);
        } else {
            emit errorOccurred(err);
            emit downloadFinished(false);
        }

        dl->deleteLater();
        emit isDownloadingChanged();
    });

    connect(dl, &FileDownloader::canceled, this, [this, dl]() {
        const auto task = mActiveDownloads.take(dl);
        mModel->setDownloadingStatus(task.index, false);
        mModel->setDownloadedStatus(task.index, false);
        mModel->updateProgress(task.index, 0.0f);
        dl->deleteLater();
        emit isDownloadingChanged();
    });

    dl->start();
    emit isDownloadingChanged();
}

void ModelTypeManager::cancelDownload(const QString& url) {
    for (auto it = mActiveDownloads.begin(); it != mActiveDownloads.end(); ++it) {
        if (it.key()->url().toString() == url) {
            it.key()->cancel();
            return;
        }
    }
}

void ModelTypeManager::deleteModel(const QString& url) {
    const QString path = storagePath(url);
    if (QFile::exists(path) && QFile::remove(path)) {
        syncDownloadedState();

        if (mSelectedPath == path) {
            mSelectedPath.clear();
            mModel->setExclusiveDefault(-1);
            emit selectedPathChanged(QString());
        }
    }
}

QString ModelTypeManager::storagePath(const QString& url) const {
    return mBaseDir + "/" + QUrl(url).fileName();
}

bool ModelTypeManager::isDownloaded(const QString& url) const {
    return QFile::exists(storagePath(url));
}

void ModelTypeManager::syncDownloadedState() {
    for (int i = 0; i < mModel->rowCount(); ++i) {
        const bool exists = QFile::exists(storagePath(mModel->getItem(i).url));
        mModel->setDownloadedStatus(i, exists);
        mModel->updateProgress(i, exists ? 1.0f : 0.0f);
    }
}

void ModelTypeManager::cleanLeftovers() {
    QDir dir(mBaseDir);
    for (const QString& f : dir.entryList({ "*.part" }))
        dir.remove(f);
}
