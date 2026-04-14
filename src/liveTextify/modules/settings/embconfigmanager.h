#pragma once
#include "liveTextify/modules/engines/common.h"
#include <QObject>

class EmbConfigManager: public QObject{
    Q_OBJECT
    Q_PROPERTY(int threadCount READ threadCount WRITE setThreadCount NOTIFY threadCountChanged FINAL)
    Q_PROPERTY(int contextLength READ contextLength WRITE setContextLength NOTIFY contextLengthChanged FINAL)

public:
    explicit EmbConfigManager(QObject * parent = nullptr);



    // Getters
    int threadCount()    const {   return mConfig.nThreads; }
    int contextLength()  const { return mConfig.nCtx; }

    // Setters
    void setThreadCount(int);
    void setContextLength(int);



    // State management
    const EmbConfig& config() const { return mConfig; }
    Q_INVOKABLE void resetToDefaults();

    void loadFromSettings();
    void saveToSettings();
signals:
    void threadCountChanged();
    void contextLengthChanged();
private:
    EmbConfig mConfig;
};
