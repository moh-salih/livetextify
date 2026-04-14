import QtQuick
import QtQuick.Layouts
import LiveTextify

SettingsSection {
    id: root
    title: "Interface"
    themeColor: Theme.secondary

    // Dependencies
    property bool isDarkMode: true
    property bool showWaveform: true

    SettingsRow {
        Layout.fillWidth: true
        title: "Dark Mode"
        description: "Switch between Obsidian and Daylight themes."

        SettingsToggle {
            checked: root.isDarkMode
            onCheckedChanged: root.isDarkMode = checked
            themeColor: Theme.primary
        }
    }

    SettingsRow {
        Layout.fillWidth: true
        title: "Waveform Visualization"
        description: "Real-time neural audio processing visualization."

        SettingsToggle {
            checked: root.showWaveform
            onCheckedChanged: root.showWaveform = checked
            themeColor: Theme.primary
        }
    }
}
