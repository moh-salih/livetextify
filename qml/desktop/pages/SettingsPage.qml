import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify
import "../components" as Components
import "../singletons"

Page {
    id: root

    property int currentConfigIndex: 0

    function handleTabClicked(index) {
        root.currentConfigIndex = index
    }

    background: Components.SettingsBackground {}

    footer: Components.SettingsFooterActions {
        id: fixedFooter

        // Connect UI actions directly to the C++ settings manager methods
        onDiscardClicked: {
            console.log("Discarding local changes, reloading from backend.")
            AppState.settingsMgr.reload();
        }
        onSyncClicked: {
            console.log("Syncing preferences to backend.")
            AppState.settingsMgr.save();
        }
    }

    ScrollView {
        anchors.fill: parent
        clip: true
        contentWidth: availableWidth
        bottomPadding: 40

        ColumnLayout {
            width: Math.min(1000, parent.width - 48)
            anchors.horizontalCenter: parent.horizontalCenter
            y: 40
            spacing: 48

            Components.SettingsHeader {
                Layout.fillWidth: true
            }

            GridLayout {
                Layout.fillWidth: true
                columns: root.width > 900 ? 12 : 1
                rowSpacing: 40
                columnSpacing: 40

                ColumnLayout {
                    Layout.columnSpan: root.width > 900 ? 4 : 1
                    Layout.alignment: Qt.AlignTop
                    Layout.fillWidth: true
                    Layout.preferredWidth: root.width > 900 ? 300 : -1
                    spacing: 24

                    Components.SettingsNavMenu {
                        currentIndex: root.currentConfigIndex
                        onTabClicked: (index) => root.handleTabClicked(index)
                        tabs: [
                            { icon: "graphic_eq",   text: "Audio",          visible: true   },
                            { icon: "palette",      text: "Appearance",     visible: false  },
                            { icon: "mic",          text: "STT Engine",     visible: true   },
                            { icon: "neurology",    text: "LLM Engine",     visible: true   },
                            { icon: "hub",          text: "Embedding",      visible: true   },
                            { icon: "memory",       text: "RAG & Memory",   visible: true   },
                            { icon: "database",     text: "Storage",        visible: false  }
                        ]

                    }

                    Components.ProLicenseCard {}
                }

                // Components are wired directly to AppState.settingsMgr mapped C++ properties
                StackLayout {
                    Layout.columnSpan: root.width > 900 ? 8 : 1
                    Layout.alignment: Qt.AlignTop
                    Layout.fillWidth: true

                    currentIndex: root.currentConfigIndex



                    Components.SettingsAudio {
                        Layout.fillWidth: true; Layout.alignment: Qt.AlignTop

                        sampleRate: AppState.audioConfig.audioSampleRate.toString()
                        channels: AppState.audioConfig.audioChannels.toString()
                        stepMs: AppState.audioConfig.step.toString()
                        keepMs: AppState.audioConfig.keep.toString()
                        maxWindow: AppState.audioConfig.maxWindow.toString()

                        onSampleRateChanged: AppState.audioConfig.audioSampleRate = parseInt(sampleRate)
                        onChannelsChanged: AppState.audioConfig.audioChannels = parseInt(channels)
                        onStepMsChanged: AppState.audioConfig.step = parseInt(stepMs)
                        onKeepMsChanged: AppState.audioConfig.keep = parseInt(keepMs)
                        onMaxWindowChanged: AppState.audioConfig.maxWindow = parseInt(maxWindow)
                    }

                    Components.SettingsAppearance {
                        Layout.fillWidth: true; Layout.alignment: Qt.AlignTop

                        fontFamily: AppState.settingsMgr.interfaceFontFamily !== undefined ? AppState.settingsMgr.interfaceFontFamily : "Manrope (Sans)"
                        isDarkMode: AppState.settingsMgr.interfaceDarkMode !== undefined ? AppState.settingsMgr.interfaceDarkMode : true

                        onFontFamilyChanged: AppState.settingsMgr.interfaceFontFamily = fontFamily
                        onIsDarkModeChanged: AppState.settingsMgr.interfaceDarkMode = isDarkMode
                    }

                    Components.SettingsSTT {
                        Layout.fillWidth: true; Layout.alignment: Qt.AlignTop

                        useGpu: AppState.sttConfig.useGpu
                        autoLang: AppState.sttConfig.autoDetectLanguage
                        translate: AppState.sttConfig.translate
                        languageCode: AppState.sttConfig.language
                        threadCount: AppState.sttConfig.threadCount
                        maxTokens: AppState.sttConfig.maxTokens
                        temperaturePenalty: AppState.sttConfig.temperature

                        onUseGpuChanged: AppState.sttConfig.useGpu = useGpu
                        onAutoLangChanged: AppState.sttConfig.autoDetectLanguage = autoLang
                        onTranslateChanged: AppState.sttConfig.translate = translate
                        onLanguageCodeChanged: AppState.sttConfig.language = languageCode
                        onThreadCountChanged: AppState.sttConfig.threadCount = threadCount
                        onMaxTokensChanged: AppState.sttConfig.maxTokens = maxTokens
                        onTemperaturePenaltyChanged: AppState.sttConfig.temperature = temperaturePenalty
                    }


                    Components.SettingsLLM {
                        Layout.fillWidth: true; Layout.alignment: Qt.AlignTop
                        Layout.bottomMargin: 50
                        systemPrompt: AppState.llmConfig.systemPrompt
                        threadCount: AppState.llmConfig.threadCount
                        batchThreads: AppState.llmConfig.batchThreads
                        contextLength: AppState.llmConfig.contextLength
                        batchSize: AppState.llmConfig.batchSize
                        temperature: AppState.llmConfig.temperature
                        topP: AppState.llmConfig.topP
                        topK: AppState.llmConfig.topK.toString()
                        repeatPenalty: AppState.llmConfig.repeatPenalty
                        maxTokens: AppState.llmConfig.maxTokens

                        onSystemPromptChanged: AppState.llmConfig.systemPrompt = systemPrompt
                        onThreadCountChanged: AppState.llmConfig.threadCount = threadCount
                        onBatchThreadsChanged: AppState.llmConfig.batchThreads = batchThreads
                        onContextLengthChanged: AppState.llmConfig.contextLength = contextLength
                        onBatchSizeChanged: AppState.llmConfig.batchSize = batchSize
                        onTemperatureChanged: AppState.llmConfig.temperature = temperature
                        onTopPChanged: AppState.llmConfig.topP = topP
                        onTopKChanged: AppState.llmConfig.topK = parseInt(topK)
                        onRepeatPenaltyChanged: AppState.llmConfig.repeatPenalty = repeatPenalty
                        onMaxTokensChanged: AppState.llmConfig.maxTokens = maxTokens
                    }


                    Components.SettingsEmbedding {
                        Layout.fillWidth: true; Layout.alignment: Qt.AlignTop

                        // contextSize: AppState.embConfig.contextSize
                        threadCount: AppState.embConfig.threadCount
                        // gpuLayers: AppState.embConfig.gpuLayers

                        // onContextSizeChanged: AppState.embConfig.contextSize = contextSize
                        onThreadCountChanged: AppState.embConfig.threadCount = threadCount
                        // onGpuLayersChanged: AppState.embConfig.gpuLayers = gpuLayers
                    }


                    Components.SettingsRAG {
                        Layout.fillWidth: true; Layout.alignment: Qt.AlignTop

                        enableRag: AppState.ragConfig.enabled
                        minChars: AppState.ragConfig.minChunkSize.toString()
                        maxChars: AppState.ragConfig.maxChunkSize.toString()
                        dimension: AppState.ragConfig.embeddingDimension.toString()
                        fillerLimit: AppState.ragConfig.fillerThreshold
                        topKResults: AppState.ragConfig.topKResults
                        minScore: AppState.ragConfig.minSimilarityScore
                        lengthBoost: AppState.ragConfig.lengthBoostFactor

                        onEnableRagChanged: AppState.ragConfig.enabled = enableRag
                        onMinCharsChanged: AppState.ragConfig.minChunkSize = parseInt(minChars)
                        onMaxCharsChanged: AppState.ragConfig.maxChunkSize = parseInt(maxChars)
                        onDimensionChanged: AppState.ragConfig.embeddingDimension = parseInt(dimension)
                        onFillerLimitChanged: AppState.ragConfig.fillerThreshold = fillerLimit
                        onTopKResultsChanged: AppState.ragConfig.topKResults = topKResults
                        onMinScoreChanged: AppState.ragConfig.minSimilarityScore = minScore
                        onLengthBoostChanged: AppState.ragConfig.lengthBoostFactor = lengthBoost
                    }

                    Components.SettingsStorage {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop

                        localBufferGB: "12.4"
                        totalBufferGB: "50"
                        bufferPercentage: 0.248

                        onPurgeClicked: {
                            AppState.sessionSvc.deleteAll()
                        }
                    }
                }
            }
        }
    }
}
