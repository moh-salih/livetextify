#include "liveTextify/modules/models/filedownloader.h"
#include <QDir>
#include <QFileInfo>
#include <QtNetwork>

FileDownloader::FileDownloader(const QUrl &url, const QString &destPath, QNetworkAccessManager *nam, QObject *parent)
    : QObject(parent), m_url(url), m_destPath(destPath), m_nam(nam) {
    connect(qApp, &QCoreApplication::aboutToQuit, this, &FileDownloader::cancel);
}

FileDownloader::~FileDownloader() {
    cancel();
}

void FileDownloader::start() {
    if (m_reply) return;

    QFileInfo fileInfo(m_destPath);
    QDir().mkpath(fileInfo.absolutePath());

    m_file.setFileName(m_destPath + ".part");
    if (!m_file.open(QIODevice::WriteOnly)) {
        emit finished(false, "Could not open file for writing: " + m_file.errorString());
        return;
    }

    QNetworkRequest request(m_url);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    m_reply = m_nam->get(request);

    connect(m_reply, &QNetworkReply::readyRead, this, &FileDownloader::onReadyRead);
    connect(m_reply, &QNetworkReply::downloadProgress, this, &FileDownloader::onDownloadProgress);
    connect(m_reply, &QNetworkReply::finished, this, &FileDownloader::onFinished);

    emit started();
}

void FileDownloader::cancel() {
    if (m_reply) {
        m_reply->abort();
    }
}

void FileDownloader::onReadyRead() {
    if (m_reply && m_file.isOpen()) {
        m_file.write(m_reply->readAll());
    }
}

void FileDownloader::onDownloadProgress(qint64 received, qint64 total) {
    if (total > 0) {
        emit progressChanged(static_cast<float>(received) / total);
    }
}

void FileDownloader::onFinished() {
    if (!m_reply) return;

    m_file.close();

    bool success = (m_reply->error() == QNetworkReply::NoError);
    auto error = m_reply->error();

    if (error == QNetworkReply::OperationCanceledError) {
        m_file.remove();
        emit canceled();
    } else if (!success) {
        m_file.remove();
        emit finished(false, m_reply->errorString());
    } else {
        // Rename the file to remove the .part extension which was added to track download leftovers
        if (m_file.rename(m_destPath)) {
            emit finished(true, "");
        } else {
            emit finished(false, "Failed to rename file");
        }
    }

    m_reply->deleteLater();
    m_reply = nullptr;
}
