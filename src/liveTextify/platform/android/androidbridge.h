#pragma once
#include <QObject>
#include <QString>

#ifdef Q_OS_ANDROID
#include <QJniObject>

class AndroidBridge : public QObject {
    Q_OBJECT
public:
    explicit AndroidBridge(const QString& androidPackagePath, QObject *parent = nullptr);

    // Storage permissions
    Q_INVOKABLE void requestStoragePermission() const;
    Q_INVOKABLE bool isStoragePermissionGranted() const;

    // Microphone permissions
    Q_INVOKABLE void requestMicrophonePermission() const;
    Q_INVOKABLE bool isMicrophonePermissionGranted() const;

    // Combined permissions
    Q_INVOKABLE void requestAllPermissions() const;
    Q_INVOKABLE bool areAllPermissionsGranted() const;

private:
    QJniObject activity;
    QString m_androidPackagePath;
};

#endif
