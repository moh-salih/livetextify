#pragma once

#include <QObject>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QtCore>
class FileDownloader : public QObject {
    Q_OBJECT
public:
    explicit FileDownloader(const QUrl &url, const QString &destPath, QNetworkAccessManager *nam, QObject *parent = nullptr);
    ~FileDownloader();

    Q_INVOKABLE void start();
    Q_INVOKABLE void cancel();

    Q_INVOKABLE QUrl url() const { return m_url; }
    Q_INVOKABLE QString destinationPath() const { return m_destPath; }

signals:
    void started();
    void progressChanged(float progress);
    void finished(bool success, const QString &errorMsg);
    void canceled();

private slots:
    void onReadyRead();
    void onDownloadProgress(qint64 received, qint64 total);
    void onFinished();

private:
    QUrl m_url;
    QString m_destPath;
    QNetworkAccessManager *m_nam;
    QNetworkReply *m_reply = nullptr;
    QFile m_file;
};
