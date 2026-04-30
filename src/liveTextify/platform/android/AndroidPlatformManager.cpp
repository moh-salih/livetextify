#include "liveTextify/platform/android/AndroidPlatformManager.h"
#include "liveTextify/core/Logger.h"

#include <QObject>
#include <QQmlContext>
#include <QtCore>
#include <QPermissions>


AndroidPlatformManager::AndroidPlatformManager(QObject* parent) : QObject(parent) {}


void AndroidPlatformManager::requestMicrophoneAccess() {
    QMicrophonePermission micPermission;

    switch (qApp->checkPermission(micPermission)) {
        case Qt::PermissionStatus::Undetermined:
            qApp->requestPermission(micPermission, [this](const QPermission &permission) {
                Logger::info(permission.status() == Qt::PermissionStatus::Granted ? "Microphone permission granted!" : "Microphone permission denied by user.");
            });
            break;
        case Qt::PermissionStatus::Denied:
            Logger::error("Microphone permission was previously denied. Please enable in OS settings.");
            break;
        case Qt::PermissionStatus::Granted:
            Logger::info("Microphone permission already granted.");
            break;
    }
}
