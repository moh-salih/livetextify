#pragma once
#include <QObject>


#include "liveTextify/modules/settings/sttconfigmanager.h"
#include "liveTextify/modules/settings/llmconfigmanager.h"
#include "liveTextify/modules/settings/audioconfigmanager.h"
#include "liveTextify/modules/settings/ragconfigmanager.h"
#include "liveTextify/modules/settings/embconfigmanager.h"

class SettingsManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(STTConfigManager     * sttConfigManager      READ sttConfigManager   NOTIFY sttConfigManagerChanged   FINAL)
    Q_PROPERTY(LLMConfigManager     * llmConfigManager      READ llmConfigManager   NOTIFY llmConfigManagerChanged   FINAL)
    Q_PROPERTY(AudioConfigManager   * audioConfigManager    READ audioConfigManager NOTIFY audioConfigManagerChanged FINAL)
    Q_PROPERTY(RAGConfigManager     * ragConfigManager      READ ragConfigManager   NOTIFY ragConfigManagerChanged   FINAL)
    Q_PROPERTY(EmbConfigManager     * embConfigManager      READ embConfigManager   NOTIFY embConfigManagerChanged   FINAL)
public:
    explicit SettingsManager(QObject *parent = nullptr);

    Q_INVOKABLE void save();
    Q_INVOKABLE void reload();

    STTConfigManager    * sttConfigManager()    const { return mSTTConfigManager; }
    LLMConfigManager    * llmConfigManager()    const { return mLLMConfigManager; }
    AudioConfigManager  * audioConfigManager()  const { return mAudioConfigManager; }
    RAGConfigManager    * ragConfigManager()    const { return mRAGConfigManager; }
    EmbConfigManager    * embConfigManager()    const { return mEmbConfigManager; }
signals:
    void sttConfigManagerChanged();
    void llmConfigManagerChanged();
    void audioConfigManagerChanged();
    void ragConfigManagerChanged();
    void embConfigManagerChanged();

private:
    STTConfigManager    * mSTTConfigManager;
    LLMConfigManager    * mLLMConfigManager;
    AudioConfigManager  * mAudioConfigManager;
    RAGConfigManager    * mRAGConfigManager;
    EmbConfigManager    * mEmbConfigManager;
};
