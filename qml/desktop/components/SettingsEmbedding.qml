import QtQuick
import QtQuick.Layouts
import LiveTextify

SettingsSection {
    id: root
    title: "Embedding Configuration"
    themeColor: Theme.secondary

    // Dependencies
    property int contextSize: 2048
    property int threadCount: 4
    property int gpuLayers: 32

    SettingsSlider {
        Layout.fillWidth: true
        label: "Context Size (nCtx)"
        hint: "Maximum embedding token window."
        valueText: Number(root.contextSize).toString()
        from: 512; to: 8192; stepSize: 128
        value: root.contextSize
        onValueChanged: root.contextSize = value
        themeColor: Theme.secondary
    }

    GridLayout {
        Layout.fillWidth: true
        Layout.topMargin: 16
        columns: 2
        columnSpacing: 40
        rowSpacing: 24

        SettingsSlider {
            Layout.fillWidth: true
            label: "Thread Count"
            hint: "Parallel processing threads"
            valueText: Number(root.threadCount).toString()
            from: 1; to: 16; stepSize: 1
            value: root.threadCount
            onValueChanged: root.threadCount = value
            themeColor: Theme.secondary
        }
        SettingsSlider {
            Layout.fillWidth: true
            label: "GPU Layers"
            hint: "Offload to VRAM"
            valueText: Number(root.gpuLayers).toString()
            from: 0; to: 128; stepSize: 1
            value: root.gpuLayers
            onValueChanged: root.gpuLayers = value
            themeColor: Theme.secondary
        }
    }
}
