import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify

ColumnLayout {
    id: root
    spacing: 24

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 160
        color: Qt.rgba(Theme.surfaceContainerHighest.r, Theme.surfaceContainerHighest.g, Theme.surfaceContainerHighest.b, 0.4)
        radius: 24
        border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.1)

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20

            Text {
                text: "NEURAL CONTEXT (RAG)"
                font.family: Fonts.headlineFamily
                font.pixelSize: 10
                font.bold: true
                font.letterSpacing: 2
                color: Theme.primary
            }

            Flow {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 8

                Tag { label: "Latency" }
                Tag { label: "Architecture" }
                Tag { label: "Thermal Scaling" }
                Tag { label: "Neural Gating"; dim: true }
            }
        }
    }

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 120
        color: Qt.rgba(Theme.surfaceContainerHighest.r, Theme.surfaceContainerHighest.g, Theme.surfaceContainerHighest.b, 0.4)
        radius: 24
        border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.1)

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 12

            RowLayout {
                Layout.fillWidth: true
                RowLayout {
                    spacing: 8
                    Text {
                        text: "sensors"
                        font.family: Fonts.iconFamily
                        font.pixelSize: 18
                        color: Theme.primary
                    }
                    Text {
                        text: "SIGNAL FIDELITY"
                        font.family: Fonts.bodyFamily
                        font.pixelSize: 10
                        font.bold: true
                        font.letterSpacing: 2
                        color: Theme.textOnSurfaceVariant
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Text {
                    text: "98.4%"
                    font.family: Fonts.headlineFamily
                    font.pixelSize: 24
                    font.bold: true
                    font.letterSpacing: -1
                    color: Theme.primary
                }
                Item { Layout.fillWidth: true }
                Text {
                    text: "STABILITY: HIGH"
                    font.family: Fonts.bodyFamily
                    font.pixelSize: 9
                    font.bold: true
                    font.letterSpacing: 1
                    color: Theme.textOnSurfaceVariant
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 6
                radius: 3
                color: Theme.surfaceContainerLow
                border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.1)

                Rectangle {
                    width: parent.width * 0.984
                    height: parent.height
                    radius: 3
                    gradient: Theme.primaryGradient
                }
            }
        }
    }

    Item { Layout.fillHeight: true }

    component Tag: Rectangle {
        id: tagRoot
        property string label
        property bool dim: false

        height: 26
        width: tagText.implicitWidth + 20
        radius: 13
        color: Qt.rgba(255, 255, 255, 0.05)
        border.color: Qt.rgba(255, 255, 255, 0.1)

        Text {
            id: tagText
            anchors.centerIn: parent
            text: tagRoot.label
            font.family: Fonts.bodyFamily
            font.pixelSize: 10
            font.bold: true
            color: tagRoot.dim ? Theme.textOnSurfaceVariant : Theme.textOnSurface
        }
    }
}
