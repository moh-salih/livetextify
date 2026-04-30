#pragma once
#include <QObject>


#include "liveTextify/modules/settings/sttconfigmanager.h"
#include "liveTextify/modules/settings/llmconfigmanager.h"
#include "liveTextify/modules/settings/audioconfigmanager.h"
#include "liveTextify/modules/settings/ragconfigmanager.h"
#include "liveTextify/modules/settings/embconfigmanager.h"
#include "liveTextify/modules/models/modelmanager.h"

struct SessionConfig;

class SettingsManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(STTConfigManager*   sttConfigManager   READ sttConfigManager   CONSTANT)
    Q_PROPERTY(LLMConfigManager*   llmConfigManager   READ llmConfigManager   CONSTANT)
    Q_PROPERTY(AudioConfigManager* audioConfigManager READ audioConfigManager CONSTANT)
    Q_PROPERTY(RAGConfigManager*   ragConfigManager   READ ragConfigManager   CONSTANT)
    Q_PROPERTY(EmbConfigManager*   embConfigManager   READ embConfigManager   CONSTANT)

    Q_PROPERTY(bool hasUnsavedChanges READ hasUnsavedChanges NOTIFY hasUnsavedChangesChanged)

public:
    explicit SettingsManager(QObject *parent = nullptr);

    Q_INVOKABLE void save();
    Q_INVOKABLE void reload();
    Q_INVOKABLE void discard();

    bool hasUnsavedChanges() const { return mHasUnsavedChanges; }

    STTConfigManager    * sttConfigManager()    const { return mSTTConfigManager;   }
    LLMConfigManager    * llmConfigManager()    const { return mLLMConfigManager;   }
    AudioConfigManager  * audioConfigManager()  const { return mAudioConfigManager; }
    RAGConfigManager    * ragConfigManager()    const { return mRAGConfigManager;   }
    EmbConfigManager    * embConfigManager()    const { return mEmbConfigManager;   }

public slots:
    void onDefaultModelChanged(const QString& type, const QString& path);

signals:
    void configChanged(const SessionConfig& config);
    void hasUnsavedChangesChanged();



private:
    STTConfigManager    * mSTTConfigManager;
    LLMConfigManager    * mLLMConfigManager;
    AudioConfigManager  * mAudioConfigManager;
    RAGConfigManager    * mRAGConfigManager;
    EmbConfigManager    * mEmbConfigManager;

    bool mHasUnsavedChanges = false;
    bool mDiscarding        = false;
};
