#pragma once

#include <QGuiApplication>
#include <QQmlApplicationEngine>




class SettingsManager;
class ResourceManager;
class PlatformManager;

class SessionManager;
class ModelManager;


class App : public QObject {
    Q_OBJECT
    
public:
    explicit App(int& argc, char* argv[], QObject *parent = nullptr);
    ~App();
    
    bool init();
    int exec();

private:
    void setupApplicationInfo();
    void setupControllers();
    void setupConnections();
    void loadInitialConfigs();
    
    QGuiApplication         mApp;
    QQmlApplicationEngine   mEngine;
    
    SettingsManager       * mSettingsManager    = nullptr;
    ResourceManager       * mResourceManager    = nullptr;
    PlatformManager       * mPlatformManager    = nullptr;

    SessionManager        * mSessionManager     = nullptr;
    ModelManager          * mWhisperManager     = nullptr;
    ModelManager          * mLLamaManager       = nullptr;
    ModelManager          * mEmbeddingManager   = nullptr;
};
