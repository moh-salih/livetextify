#pragma once
#include <QObject>
#include "liveTextify/types.h"


class LLMConfigManager : public QObject{
    Q_OBJECT
    Q_PROPERTY(QString systemPrompt READ systemPrompt WRITE setSystemPrompt NOTIFY systemPromptChanged FINAL)
    Q_PROPERTY(int threadCount READ threadCount WRITE setThreadCount NOTIFY threadCountChanged FINAL)
    Q_PROPERTY(int contextLength READ contextLength WRITE setContextLength NOTIFY contextLengthChanged FINAL)
    Q_PROPERTY(int batchSize READ batchSize WRITE setBatchSize NOTIFY batchSizeChanged FINAL)
    Q_PROPERTY(int maxTokens READ maxTokens WRITE setMaxTokens NOTIFY maxTokensChanged FINAL)
    Q_PROPERTY(int topK READ topK WRITE setTopK NOTIFY topKChanged FINAL)
    Q_PROPERTY(int batchThreads READ batchThreads WRITE setBatchThreads NOTIFY batchThreadsChanged FINAL)
    Q_PROPERTY(qreal temperature READ temperature WRITE setTemperature NOTIFY temperatureChanged FINAL)
    Q_PROPERTY(qreal topP READ topP WRITE setTopP NOTIFY topPChanged FINAL)
    Q_PROPERTY(qreal repeatPenalty READ repeatPenalty WRITE setRepeatPenalty NOTIFY repeatPenaltyChanged FINAL)
public:
    explicit LLMConfigManager(QObject * parent = nullptr);


    // Getters
    QString systemPrompt()      const { return mSystemPrompt; };
    int     threadCount()       const { return mConfig.threadCount; }
    int     contextLength()     const { return mConfig.contextLength; }
    int     batchSize()         const { return mConfig.batchSize; }
    int     maxTokens()         const { return mConfig.maxTokens; }
    int     topK()              const { return mConfig.topK; }
    int     batchThreads()      const { return mConfig.batchThreads; }
    qreal   temperature()       const { return mConfig.temperature; }
    qreal   topP()              const { return mConfig.topP; }
    qreal   repeatPenalty()     const { return mConfig.repeatPenalty; }
    QString modelPath()         const { return mConfig.modelPath; }


    // Setters
    void setSystemPrompt(const QString&);
    void setThreadCount(int);
    void setContextLength(int);
    void setBatchSize(int);
    void setMaxTokens(int);
    void setTopK(int);
    void setBatchThreads(int);
    void setTemperature(qreal);
    void setTopP(qreal);
    void setRepeatPenalty(qreal);
    void setModelPath(const QString& v);

    // state management
    const LLMConfig& config() const { return mConfig; }
    Q_INVOKABLE void resetToDefaults();
    void loadFromSettings();
    void saveToSettings();
signals:
    void systemPromptChanged();
    void threadCountChanged();
    void contextLengthChanged();
    void batchSizeChanged();
    void maxTokensChanged();
    void topKChanged();
    void batchThreadsChanged();
    void temperatureChanged();
    void topPChanged();
    void repeatPenaltyChanged();
    void configChanged();
private:
    LLMConfig mConfig;
    QString mSystemPrompt;
};


