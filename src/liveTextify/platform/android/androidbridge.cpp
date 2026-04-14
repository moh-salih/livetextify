#include "liveTextify/platform/android/androidbridge.h"

#ifdef Q_OS_ANDROID
#include <QDebug>

AndroidBridge::AndroidBridge(const QString& androidPackagePath, QObject *parent)
    : QObject(parent),
    m_androidPackagePath(androidPackagePath)
{
    activity = QJniObject::callStaticObjectMethod("org/qtproject/qt/android/QtNative", "activity", "()Landroid/app/Activity;");
    if (!activity.isValid()) {
        qWarning() << "Failed to get Android activity";
    }
}


void AndroidBridge::requestStoragePermission() const {
    if (!activity.isValid()) return;
    std::string signature = m_androidPackagePath.toStdString() + "/PermissionManager";
    QJniObject::callStaticMethod<void>(
        signature.c_str(),
        "requestStoragePermission",
        "(Landroid/app/Activity;)V",
        activity.object<jobject>()
        );
}

bool AndroidBridge::isStoragePermissionGranted() const {
    if (!activity.isValid()) return false;


    std::string signature = m_androidPackagePath.toStdString() + "/PermissionManager";
    return QJniObject::callStaticMethod<jboolean>(
        signature.c_str(),
        "isStoragePermissionGranted",
        "(Landroid/app/Activity;)Z",
        activity.object<jobject>()
        );
}

void AndroidBridge::requestMicrophonePermission() const {
    if (!activity.isValid()) return;

    std::string signature = m_androidPackagePath.toStdString() + "/PermissionManager";
    QJniObject::callStaticMethod<void>(
        signature.c_str(),
        "requestMicrophonePermission",
        "(Landroid/app/Activity;)V",
        activity.object<jobject>()
        );
}

bool AndroidBridge::isMicrophonePermissionGranted() const {
    if (!activity.isValid()) return false;

    std::string signature = m_androidPackagePath.toStdString() + "/PermissionManager";
    return QJniObject::callStaticMethod<jboolean>(
        signature.c_str(),
        "isMicrophonePermissionGranted",
        "(Landroid/app/Activity;)Z",
        activity.object<jobject>()
        );
}

void AndroidBridge::requestAllPermissions() const {
    if (!activity.isValid()) return;

    std::string signature = m_androidPackagePath.toStdString() + "/PermissionManager";
    QJniObject::callStaticMethod<void>(
        signature.c_str(),
        "requestAllPermissions",
        "(Landroid/app/Activity;)V",
        activity.object<jobject>()
        );
}

bool AndroidBridge::areAllPermissionsGranted() const {
    if (!activity.isValid()) return false;

    std::string signature = m_androidPackagePath.toStdString() + "/PermissionManager";
    return QJniObject::callStaticMethod<jboolean>(
        signature.c_str(),
        "areAllPermissionsGranted",
        "(Landroid/app/Activity;)Z",
        activity.object<jobject>()
        );
}

#endif // Q_OS_ANDROID


