import QtQuick
import QtQuick.Layouts
import LiveTextify

SettingsSection {
    id: root
    title: "RAG & Memory"
    themeColor: Theme.tertiary

    // Dependencies
    property bool enableRag: true
    property string minChars: "50"
    property string maxChars: "2000"
    property string dimension: "768"
    property int fillerLimit: 5
    property int topKResults: 3
    property real minScore: 0.65
    property real lengthBoost: 1.2

    RowLayout {
        Layout.fillWidth: true
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4
            Text { text: "Enable RAG"; font.family: Fonts.headlineFamily; font.pixelSize: 16; font.bold: true; color: Theme.textOnSurface }
            Text { text: "Enable retrieval augmented generation for context retrieval."; font.family: Fonts.bodyFamily; font.pixelSize: 14; color: Theme.textOnSurfaceVariant }
        }
        SettingsToggle {
            checked: root.enableRag
            onCheckedChanged: root.enableRag = checked
            themeColor: Theme.primary
        }
    }

    GridLayout {
        Layout.fillWidth: true
        Layout.topMargin: 24
        columns: 3
        columnSpacing: 24

        SettingsInput { label: "Min Chars"; text: root.minChars; onTextChanged: root.minChars = text; Layout.fillWidth: true }
        SettingsInput { label: "Max Chars"; text: root.maxChars; onTextChanged: root.maxChars = text; Layout.fillWidth: true }
        SettingsInput { label: "Dimension"; text: root.dimension; onTextChanged: root.dimension = text; Layout.fillWidth: true }
    }

    Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.2); Layout.topMargin: 16; Layout.bottomMargin: 16 }

    SettingsSlider { label: "Filler Limit"; valueText: Number(root.fillerLimit).toString(); from: 1; to: 20; value: root.fillerLimit; onValueChanged: root.fillerLimit = value; themeColor: Theme.primary; Layout.fillWidth: true }
    SettingsSlider { label: "Top K Results"; valueText: Number(root.topKResults).toString(); from: 1; to: 20; value: root.topKResults; onValueChanged: root.topKResults = value; themeColor: Theme.primary; Layout.fillWidth: true }

    GridLayout {
        Layout.fillWidth: true
        Layout.topMargin: 16
        columns: 2
        columnSpacing: 32

        SettingsSlider { label: "Min Score"; valueText: Number(root.minScore).toFixed(2); from: 0.0; to: 1.0; stepSize: 0.05; value: root.minScore; onValueChanged: root.minScore = value; themeColor: Theme.primary; Layout.fillWidth: true }
        SettingsSlider { label: "Length Boost"; valueText: Number(root.lengthBoost).toFixed(2); from: 0.0; to: 2.0; stepSize: 0.1; value: root.lengthBoost; onValueChanged: root.lengthBoost = value; themeColor: Theme.primary; Layout.fillWidth: true }
    }
}
