#pragma once

#include <QObject>

class AndroidPlatformManager : public QObject {
    Q_OBJECT
    
public:
    explicit AndroidPlatformManager(QObject* parent = nullptr);
    ~AndroidPlatformManager() = default;
    
    // TODO: Add Android-specific methods here
    // Example:
    // Q_INVOKABLE void requestPermission(const QString& permission);
    // Q_INVOKABLE bool hasPermission(const QString& permission);
    // Q_INVOKABLE void shareText(const QString& text);
    // Q_INVOKABLE void showToast(const QString& message);
};