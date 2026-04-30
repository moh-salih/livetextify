import QtQuick
import QtQuick.Layouts
import LiveTextify

SettingsSection {
    id: root
    title: "LLM Engine"
    themeColor: Theme.tertiary

    // Dependencies
    property string systemPrompt: ""
    property int threadCount: 8
    property int batchThreads: 8
    property int contextLength: 4096
    property int batchSize: 512
    property real temperature: 0.7
    property real topP: 0.9
    property string topK: "40"
    property string repeatPenalty: "1.1"
    property int maxTokens: 256

    ColumnLayout {
        Layout.fillWidth: true
        spacing: 8
        Text { text: "System Prompt"; font.family: Fonts.headlineFamily; font.pixelSize: 14; font.bold: true; color: Theme.textOnSurface }
        SettingsTextArea {
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            text: root.systemPrompt
            onTextChanged: root.systemPrompt = text
        }
    }

    GridLayout {
        Layout.fillWidth: true
        Layout.topMargin: 16
        columns: 2
        columnSpacing: 40
        rowSpacing: 32

        SettingsSlider { label: "Thread Count"; valueText: Number(root.threadCount).toString(); from: 1; to: 32; value: root.threadCount; onValueChanged: root.threadCount = value; themeColor: Theme.tertiary; Layout.fillWidth: true }
        SettingsSlider { label: "Batch Threads"; valueText: Number(root.batchThreads).toString(); from: 1; to: 32; value: root.batchThreads; onValueChanged: root.batchThreads = value; themeColor: Theme.tertiary; Layout.fillWidth: true }
        SettingsSlider { label: "Context Length"; valueText: Number(root.contextLength).toString(); from: 0; to: 32768; stepSize: 512; value: root.contextLength; onValueChanged: root.contextLength = value; themeColor: Theme.primary; Layout.fillWidth: true }
        SettingsSlider { label: "Batch Size"; valueText: Number(root.batchSize).toString(); from: 128; to: 2048; stepSize: 128; value: root.batchSize; onValueChanged: root.batchSize = value; themeColor: Theme.primary; Layout.fillWidth: true }
    }

    Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.2); Layout.topMargin: 16; Layout.bottomMargin: 16 }

    SettingsSlider { label: "Temperature"; valueText: Number(root.temperature).toFixed(2); from: 0.1; to: 1.5; stepSize: 0.1; value: root.temperature; onValueChanged: root.temperature = value; themeColor: Theme.primary; Layout.fillWidth: true }
    SettingsSlider { label: "Top P"; valueText: Number(root.topP).toFixed(2); from: 0.0; to: 1.0; stepSize: 0.05; value: root.topP; onValueChanged: root.topP = value; themeColor: Theme.primary; Layout.fillWidth: true }

    GridLayout {
        Layout.fillWidth: true
        Layout.topMargin: 16
        columns: 3
        columnSpacing: 24

        SettingsInput { label: "Top K"; text: root.topK; onTextChanged: root.topK = text; Layout.fillWidth: true }
        SettingsInput { label: "Repeat Penalty"; text: root.repeatPenalty; onTextChanged: root.repeatPenalty = text; Layout.fillWidth: true }
        SettingsSlider { label: "Max Tokens"; valueText: Number(root.maxTokens).toString(); from: 16; to: 1024; stepSize: 16; value: root.maxTokens; onValueChanged: root.maxTokens = value; themeColor: Theme.primary; Layout.fillWidth: true }
    }
}
