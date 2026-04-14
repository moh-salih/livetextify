#pragma once
#include <QObject>
#include "liveTextify/modules/engines/common.h"

class STTConfigManager: public QObject{
    Q_OBJECT
    Q_PROPERTY(int maxTokens READ maxTokens WRITE setMaxTokens NOTIFY maxTokensChanged FINAL)
    Q_PROPERTY(bool useGpu READ useGpu WRITE setUseGpu NOTIFY useGpuChanged FINAL)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged FINAL)
    Q_PROPERTY(int threadCount READ threadCount WRITE setThreadCount NOTIFY threadCountChanged FINAL)
    Q_PROPERTY(qreal temperature READ temperature WRITE setTemperature NOTIFY temperatureChanged FINAL)
    Q_PROPERTY(bool translate READ translate WRITE setTranslate NOTIFY translateChanged FINAL)
    Q_PROPERTY(bool autoDetectLanguage READ autoDetectLanguage WRITE setAutoDetectLanguage NOTIFY autoDetectLanguageChanged FINAL)
public:
    explicit STTConfigManager(QObject * parent = nullptr);


    // Setters
    void setMaxTokens(int);
    void setUseGpu(bool);
    void setLanguage(const QString&);
    void setThreadCount(int);
    void setTemperature(qreal);
    void setTranslate(bool);
    void setAutoDetectLanguage(bool);

    // Getters
    int     maxTokens()             const { return mConfig.maxTokens; }
    bool    useGpu()                const { return mConfig.useGpu; }
    QString language()              const { return mConfig.language; }
    int     threadCount()           const { return mConfig.threadCount; }
    qreal   temperature()           const { return mConfig.temperature; }
    bool    translate()             const { return mConfig.translate; }
    bool    autoDetectLanguage()    const { return mConfig.autoDetectLanguage; }
    // for internal use
    const STTConfig& config() const { return mConfig; }
    // Reset to defaults (useful for UI "Reset to Defaults" button)
    Q_INVOKABLE void resetToDefaults();




    // state management
    void loadFromSettings();
    void saveToSettings();
signals:
    void maxTokensChanged();
    void useGpuChanged();
    void languageChanged();
    void threadCountChanged();
    void temperatureChanged();
    void translateChanged();
    void autoDetectLanguageChanged();
private:
    STTConfig mConfig;
};

