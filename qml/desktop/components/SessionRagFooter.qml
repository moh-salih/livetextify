import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify

GridLayout {
    id: root
    columns: 2
    columnSpacing: 24

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 140
        color: Qt.rgba(Theme.surfaceContainerHighest.r, Theme.surfaceContainerHighest.g, Theme.surfaceContainerHighest.b, 0.4)
        radius: 24
        border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.2)

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 24

            Text {
                text: "CURRENT ANALYSIS (RAG)"
                font.family: "Space Grotesk"
                font.pixelSize: 10
                font.bold: true
                font.letterSpacing: 2
                color: Theme.primary
            }

            Flow {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 10

                Tag { label: "Latency" }
                Tag { label: "RAG Architecture" }
                Tag { label: "Thermal Scaling" }
                Tag {
                    label: "Neural Gating"
                    dim: true
                }
            }
        }
    }

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 140
        color: Qt.rgba(Theme.surfaceContainerHighest.r, Theme.surfaceContainerHighest.g, Theme.surfaceContainerHighest.b, 0.4)
        radius: 24
        border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.2)

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 16

            RowLayout {
                Layout.fillWidth: true
                RowLayout {
                    spacing: 8
                    Text {
                        text: "sensors"
                        font.family: "Material Symbols Outlined"
                        font.pixelSize: 16
                        color: Theme.primary
                    }
                    Text {
                        text: "SIGNAL FIDELITY"
                        font.family: "Manrope"
                        font.pixelSize: 10
                        font.bold: true
                        font.letterSpacing: 2
                        color: Theme.textOnSurfaceVariant
                    }
                }
                Item {
                    Layout.fillWidth: true
                }
                Text {
                    text: "98.4%"
                    font.family: "Space Grotesk"
                    font.pixelSize: 14
                    font.bold: true
                    font.letterSpacing: 1
                    color: Theme.primary
                }
            }

            Item {
                Layout.fillHeight: true
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 8
                radius: 4
                color: Theme.surfaceContainerLow
                border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.4)

                Rectangle {
                    width: parent.width * 0.984
                    height: 6
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 1
                    radius: 3
                    gradient: Theme.primaryGradient
                }
            }
        }
    }

    component Tag: Rectangle {
        id: tagRoot
        property string label
        property bool dim: false

        height: 28
        width: tagText.implicitWidth + 24
        radius: 14
        color: Qt.rgba(255, 255, 255, 0.05)
        border.color: Qt.rgba(255, 255, 255, 0.1)

        Text {
            id: tagText
            anchors.centerIn: parent
            text: tagRoot.label
            font.family: "Manrope"
            font.pixelSize: 11
            font.bold: true
            color: tagRoot.dim ? Theme.textOnSurfaceVariant : Theme.textOnSurface
        }
    }
}
