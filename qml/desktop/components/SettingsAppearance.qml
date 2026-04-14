import QtQuick
import QtQuick.Layouts
import LiveTextify

SettingsSection {
    id: root
    title: "Appearance"
    themeColor: Theme.primary

    // Dependencies
    property string fontFamily: ""
    property bool isDarkMode: false

    RowLayout {
        Layout.fillWidth: true
        spacing: 24

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4
            Text { text: "Font Family"; font.family: Fonts.headlineFamily; font.pixelSize: 16; font.bold: true; color: Theme.textOnSurface }
            Text { text: "Primary transcription display typeface."; font.family: Fonts.bodyFamily; font.pixelSize: 14; color: Theme.textOnSurfaceVariant }
        }
        SettingsInput {
            Layout.preferredWidth: 180
            text: root.fontFamily
            onTextChanged: root.fontFamily = text
            isDropdown: true
        }
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: 24

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4
            Text { text: "Dark Mode"; font.family: Fonts.headlineFamily; font.pixelSize: 16; font.bold: true; color: Theme.textOnSurface }
            Text { text: "Obsidian interface mode."; font.family: Fonts.bodyFamily; font.pixelSize: 14; color: Theme.textOnSurfaceVariant }
        }
        SettingsToggle {
            checked: root.isDarkMode
            onCheckedChanged: root.isDarkMode = checked
            themeColor: Theme.primary
        }
    }
}
