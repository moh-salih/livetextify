#pragma once

#include <QObject>
#include <memory>
#include "liveTextify/platform/android/AndroidPlatformManager.h"
#include "liveTextify/platform/ios/IOSPlatformManager.h"


class PlatformManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isAndroid READ isAndroid CONSTANT)
    Q_PROPERTY(bool isIOS READ isIOS CONSTANT)
public:
    static PlatformManager& instance();
    
    // Platform checks
    bool isAndroid() const;
    bool isIOS() const;
    
    // Platform-specific getters (returns nullptr if not available)
    AndroidPlatformManager* android();
    IOSPlatformManager* ios();
    
private:
    PlatformManager() = default;
    ~PlatformManager() = default;
    
    std::unique_ptr<AndroidPlatformManager> mAndroid;
    std::unique_ptr<IOSPlatformManager> mIOS;
};