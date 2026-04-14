#pragma once

#include <QObject>

class IOSPlatformManager : public QObject {
    Q_OBJECT
    
public:
    explicit IOSPlatformManager(QObject* parent = nullptr);
    ~IOSPlatformManager() = default;
    
    // TODO: Add iOS-specific methods here
    // Example:
    // Q_INVOKABLE void requestPermission(const QString& permission);
    // Q_INVOKABLE void shareText(const QString& text);
    // Q_INVOKABLE void showAlert(const QString& title, const QString& message);
};