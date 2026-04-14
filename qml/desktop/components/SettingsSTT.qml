import QtQuick
import QtQuick.Layouts
import LiveTextify

SettingsSection {
    id: root
    title: "STT Engine"
    themeColor: Theme.secondary

    // Dependencies
    property bool useGpu: true
    property bool autoLang: false
    property bool translate: false
    property string languageCode: "en"
    property int threadCount: 4
    property int maxTokens: 128
    property real temperaturePenalty: 0.0

    GridLayout {
        Layout.fillWidth: true
        columns: 3
        columnSpacing: 32

        RowLayout {
            Layout.fillWidth: true
            Text { text: "Use GPU"; font.family: Fonts.headlineFamily; font.pixelSize: 14; font.bold: true; color: Theme.textOnSurface; Layout.fillWidth: true }
            SettingsToggle { checked: root.useGpu; onCheckedChanged: root.useGpu = checked; themeColor: Theme.primary }
        }
        RowLayout {
            Layout.fillWidth: true
            Text { text: "Auto Lang"; font.family: Fonts.headlineFamily; font.pixelSize: 14; font.bold: true; color: Theme.textOnSurface; Layout.fillWidth: true }
            SettingsToggle { checked: root.autoLang; onCheckedChanged: root.autoLang = checked; themeColor: Theme.primary }
        }
        RowLayout {
            Layout.fillWidth: true
            Text { text: "Translate"; font.family: Fonts.headlineFamily; font.pixelSize: 14; font.bold: true; color: Theme.textOnSurface; Layout.fillWidth: true }
            SettingsToggle { checked: root.translate; onCheckedChanged: root.translate = checked; themeColor: Theme.primary }
        }
    }

    GridLayout {
        Layout.fillWidth: true
        Layout.topMargin: 24
        columns: 2
        columnSpacing: 40
        rowSpacing: 32

        SettingsInput { label: "Language Code"; text: root.languageCode; onTextChanged: root.languageCode = text; placeholderText: "en"; Layout.fillWidth: true }
        SettingsSlider { label: "Thread Count"; valueText: Number(root.threadCount).toString(); from: 1; to: 16; stepSize: 1; value: root.threadCount; onValueChanged: root.threadCount = value; themeColor: Theme.secondary; Layout.fillWidth: true }
        SettingsSlider { label: "Max Tokens"; valueText: Number(root.maxTokens).toString(); from: 16; to: 512; stepSize: 16; value: root.maxTokens; onValueChanged: root.maxTokens = value; themeColor: Theme.primary; Layout.fillWidth: true }
        SettingsSlider { label: "Temp (Penalty)"; valueText: Number(root.temperaturePenalty).toFixed(1); from: 0.0; to: 1.0; stepSize: 0.1; value: root.temperaturePenalty; onValueChanged: root.temperaturePenalty = value; themeColor: Theme.primary; Layout.fillWidth: true }
    }
}
