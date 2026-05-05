import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects

Popup {
    id: root

    // --- Configuration Properties ---
    enum Type { Info, Success, Warning, Error }
    property int dialogType: MessageDialog.Error

    property string title: "System Exception"
    property string text: "An error occurred."
    property string primaryText: "Dismiss"
    property string secondaryText: "" // If provided, shows a cancel/secondary button

    // --- Dynamic Theming ---
    readonly property color accentColor: {
        switch (dialogType) {
            case MessageDialog.Error: return "#ff716c";   // error
            case MessageDialog.Warning: return "#ffb74d"; // warning
            case MessageDialog.Success: return "#8ff5ff"; // primary
            default: return "#65afff";                    // tertiary (Info)
        }
    }

    readonly property color accentMidColor: {
        switch (dialogType) {
            case MessageDialog.Error: return "#9f0519";   // error-container
            case MessageDialog.Warning: return "#f57c00"; // orange
            case MessageDialog.Success: return "#65afff"; // tertiary
            default: return "#8ff5ff";                    // primary
        }
    }

    readonly property string iconGlyph: {
        switch (dialogType) {
            case MessageDialog.Error: return "report";
            case MessageDialog.Warning: return "warning";
            case MessageDialog.Success: return "check_circle";
            default: return "info";
        }
    }

    // --- Signals ---
    signal accepted()
    signal rejected()

    // --- Popup Settings ---
    width: 480
    height: implicitHeight
    anchors.centerIn: Overlay.overlay
    modal: true
    dim: true
    closePolicy: Popup.CloseOnEscape | Popup.NoAutoClose

    // Smooth Enter/Exit Animations
    enter: Transition {
        ParallelAnimation {
            NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 250; easing.type: Easing.OutCubic }
            NumberAnimation { property: "scale"; from: 0.95; to: 1.0; duration: 250; easing.type: Easing.OutBack }
        }
    }
    exit: Transition {
        ParallelAnimation {
            NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 200; easing.type: Easing.InCubic }
            NumberAnimation { property: "scale"; from: 1.0; to: 0.95; duration: 200; easing.type: Easing.InCubic }
        }
    }

    // Dark blurred backdrop
    Overlay.modal: Rectangle {
        color: "#99000000"
        layer.enabled: true
        layer.effect: MultiEffect { blurEnabled: true; blurMax: 48; blur: 0.6 }
    }

    background: Item {
        // Deep Drop Shadow
        Rectangle {
            anchors.fill: panel
            color: "black"
            radius: 16
            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "#000000"
                shadowOpacity: 0.8
                shadowBlur: 60
                shadowVerticalOffset: 15
            }
        }

        // Main glass panel
        Rectangle {
            id: panel
            anchors.fill: parent
            color: "#B30f1930"
            radius: 16
            border.color: "#2640485d"
        }

        // Background Decorative Element
        Rectangle {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: -16
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width * 0.8
            height: 32
            color: root.accentColor
            opacity: 0.05
            layer.enabled: true
            layer.effect: MultiEffect { blurEnabled: true; blurMax: 64; blur: 1.0 }
        }
    }

    // --- Content ---
    contentItem: ColumnLayout {
        spacing: 24
        Layout.margins: 24 // Nice clean padding around the whole dialog

        // 1. Error Icon Cluster
        Item {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 80
            Layout.preferredHeight: 80
            Layout.topMargin: 8

            // Outer pulsating glow rings
            Rectangle {
                anchors.centerIn: parent
                width: 120; height: 120
                radius: width / 2
                color: root.accentColor
                opacity: 0.10
                layer.enabled: true
                layer.effect: MultiEffect { blurEnabled: true; blurMax: 48; blur: 1.0 }

                SequentialAnimation on scale {
                    loops: Animation.Infinite
                    NumberAnimation { from: 1.0; to: 1.05; duration: 2000; easing.type: Easing.InOutSine }
                    NumberAnimation { from: 1.05; to: 1.0; duration: 2000; easing.type: Easing.InOutSine }
                }
            }

            // Icon Container
            Rectangle {
                anchors.fill: parent
                radius: width / 2
                color: "#000000"
                border.color: Qt.rgba(root.accentColor.r, root.accentColor.g, root.accentColor.b, 0.2)
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    font.family: "Material Symbols Outlined"
                    font.pixelSize: 36
                    color: root.accentColor
                    text: root.iconGlyph

                    // Error glow
                    layer.enabled: true
                    layer.effect: MultiEffect {
                        shadowEnabled: true; shadowColor: root.accentColor
                        shadowBlur: 1.5; shadowOpacity: 0.4
                    }
                }
            }
        }

        // 2. Titles and Message
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 16

            Label {
                Layout.alignment: Qt.AlignHCenter
                text: root.title
                font.family: "Space Grotesk"
                font.pixelSize: 28
                font.bold: true
                color: "#dee5ff"
            }

            // Centered Text block
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: Math.max(80, messageText.implicitHeight + 32)
                color: "#80000000"
                radius: 12
                border.color: "#1A40485d"

                Text {
                    id: messageText
                    anchors.fill: parent
                    anchors.margins: 16
                    text: root.text
                    textFormat: Text.RichText
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    font.family: "Manrope"
                    font.pixelSize: 14
                    font.weight: Font.Medium
                    color: "#a3aac4"
                    lineHeight: 1.4
                }
            }
        }

        // 3. Action Buttons (Right-Aligned)
        RowLayout {
            Layout.alignment: Qt.AlignRight
            Layout.topMargin: 8
            spacing: 12

            // Optional Secondary Button
            Button {
                visible: root.secondaryText !== ""
                Layout.preferredWidth: 100
                Layout.preferredHeight: 40

                background: Rectangle {
                    radius: 8
                    color: "transparent"
                    border.color: Qt.rgba(root.accentColor.r, root.accentColor.g, root.accentColor.b, 0.4)
                    border.width: 1
                }

                contentItem: Text {
                    text: root.secondaryText
                    font.family: "Space Grotesk"
                    font.bold: true
                    font.pixelSize: 14
                    color: root.accentColor
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    root.rejected()
                    root.close()
                }

                scale: pressed ? 0.95 : 1.0
                Behavior on scale { NumberAnimation { duration: 100 } }
            }

            // Primary Action Button
            Button {
                id: actionBtn
                Layout.preferredWidth: 130
                Layout.preferredHeight: 40

                background: Rectangle {
                    radius: 8
                    gradient: Gradient {
                        orientation: Gradient.Horizontal
                        GradientStop { position: 0.0; color: root.accentColor }
                        GradientStop { position: 0.5; color: root.accentMidColor }
                        GradientStop { position: 1.0; color: root.accentColor }
                    }

                    layer.enabled: true
                    layer.effect: MultiEffect {
                        shadowEnabled: true
                        shadowColor: root.accentColor
                        shadowOpacity: 0.3
                        shadowBlur: 1.0
                        shadowVerticalOffset: 2
                    }
                }

                contentItem: RowLayout {
                    spacing: 8
                    anchors.centerIn: parent

                    Label {
                        text: root.primaryText
                        font.family: "Space Grotesk"
                        font.bold: true
                        font.pixelSize: 14
                        color: "#ffffff"
                    }

                    Text {
                        font.family: "Material Symbols Outlined"
                        text: root.dialogType === MessageDialog.Error ? "close" : "check"
                        font.pixelSize: 16
                        color: "#ffffff"
                    }
                }

                onClicked: {
                    root.accepted()
                    root.close()
                }

                scale: pressed ? 0.95 : 1.0
                Behavior on scale { NumberAnimation { duration: 100 } }
            }
        }
    }
}
