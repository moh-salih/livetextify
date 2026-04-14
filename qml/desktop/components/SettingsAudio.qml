import QtQuick
import QtQuick.Layouts
import LiveTextify

SettingsSection {
    id: root
    title: "Audio IO"
    themeColor: Theme.primary

    // Dependencies
    property string sampleRate: ""
    property string channels: ""
    property string stepMs: ""
    property string keepMs: ""
    property string maxWindow: ""

    GridLayout {
        Layout.fillWidth: true
        columns: 2
        columnSpacing: 32
        rowSpacing: 32

        SettingsInput { label: "Sample Rate"; hint: "Input fidelity"; text: root.sampleRate; onTextChanged: root.sampleRate = text; isDropdown: true; Layout.fillWidth: true }
        SettingsInput { label: "Channels"; hint: "Mono/Stereo"; text: root.channels; onTextChanged: root.channels = text; Layout.fillWidth: true }
        SettingsInput { label: "Step (ms)"; hint: "Analysis frequency"; text: root.stepMs; onTextChanged: root.stepMs = text; Layout.fillWidth: true }
        SettingsInput { label: "Keep (ms)"; text: root.keepMs; onTextChanged: root.keepMs = text; Layout.fillWidth: true }
        SettingsInput { label: "Max Window (ms)"; text: root.maxWindow; onTextChanged: root.maxWindow = text; Layout.fillWidth: true }
    }
}
