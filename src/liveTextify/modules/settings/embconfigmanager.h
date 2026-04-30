#pragma once

#include <QObject>
#include "liveTextify/types.h"

class EmbConfigManager: public QObject{
    Q_OBJECT
    Q_PROPERTY(int threadCount READ threadCount WRITE setThreadCount NOTIFY threadCountChanged FINAL)
    Q_PROPERTY(int contextLength READ contextLength WRITE setContextLength NOTIFY contextLengthChanged FINAL)

public:
    explicit EmbConfigManager(QObject * parent = nullptr);



    // Getters
    int threadCount()    const {   return mConfig.nThreads; }
    int contextLength()  const { return mConfig.nCtx; }
    QString modelPath()  const { return mConfig.modelPath; }

    // Setters
    void setThreadCount(int);
    void setContextLength(int);
    void setModelPath(const QString& v);


    // State management
    const EmbConfig& config() const { return mConfig; }
    Q_INVOKABLE void resetToDefaults();

    void loadFromSettings();
    void saveToSettings();
signals:
    void threadCountChanged();
    void contextLengthChanged();
    void configChanged();

private:
    EmbConfig mConfig;
};
