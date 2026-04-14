#include "liveTextify/platform/PlatformManager.h"


PlatformManager& PlatformManager::instance() {
    static PlatformManager instance;
    return instance;
}

bool PlatformManager::isAndroid() const {
#ifdef Q_OS_ANDROID
    return true;
#else
    return false;
#endif
}

bool PlatformManager::isIOS() const {
#ifdef Q_OS_IOS
    return true;
#else
    return false;
#endif
}


AndroidPlatformManager* PlatformManager::android() {
#ifdef Q_OS_ANDROID
    if (!mAndroid) {
        mAndroid = std::make_unique<AndroidPlatformManager>();
    }
    return mAndroid.get();
#else
    return nullptr;
#endif
}

IOSPlatformManager* PlatformManager::ios() {
#ifdef Q_OS_IOS
    if (!mIOS) {
        mIOS = std::make_unique<IOSPlatformManager>();
    }
    return mIOS.get();
#else
    return nullptr;
#endif
}
