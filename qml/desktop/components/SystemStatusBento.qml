import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify

Rectangle {
    id: root
    color: Theme.surfaceContainerLow
    radius: 24

    // FIX: Provide explicit baseline dimensions so the Grid doesn't collapse
    implicitHeight: 280
    implicitWidth: 400

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 32
        spacing: 24

        // Top Section
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 16

            Rectangle {
                color: Qt.rgba(Theme.primary.r, Theme.primary.g, Theme.primary.b, 0.1)
                border.color: Qt.rgba(Theme.primary.r, Theme.primary.g, Theme.primary.b, 0.2)
                radius: 12
                Layout.preferredHeight: 24
                Layout.preferredWidth: edgeProcessingText.implicitWidth + 24

                Text {
                    id: edgeProcessingText
                    anchors.centerIn: parent
                    text: "EDGE PROCESSING"
                    font.family: Fonts.bodyFamily
                    font.pixelSize: Fonts.tiny
                    font.bold: true
                    font.letterSpacing: 2
                    color: Theme.primary
                }
            }

            Text {
                text: "Neural Engine Active"
                font.family: Fonts.headlineFamily
                font.pixelSize: Fonts.displaySmall
                font.bold: true
                color: Theme.textOnSurface
            }

            Text {
                text: "System is currently utilizing <font color='" + Theme.primary + "'><b>Whisper-V3-Turbo</b></font> for audio ingestion and <font color='" + Theme.secondary + "'><b>LLaMA 3 8B</b></font> for contextual analysis. 14.2GB VRAM utilized."
                font.family: Fonts.bodyFamily
                font.pixelSize: Fonts.normal
                color: Theme.textOnSurfaceVariant
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                textFormat: Text.RichText
                lineHeight: 1.4
            }
        }

        Item { Layout.fillHeight: true }

        // Bottom Metrics Grid
        GridLayout {
            Layout.fillWidth: true
            columns: 3
            columnSpacing: 16

            MetricBlock { label: "INFERENCE LATENCY"; value: "24ms" }
            MetricBlock { label: "TOKENS/SEC"; value: "84.2" }
            MetricBlock { label: "ACTIVE THREADS"; value: "12" }
        }
    }

    component MetricBlock: Rectangle {
        property string label
        property string value

        Layout.fillWidth: true
        Layout.preferredHeight: 80
        color: Theme.surfaceContainerLowest
        radius: 16

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 4

            Text {
                text: label
                font.family: Fonts.bodyFamily
                font.pixelSize: Fonts.tiny
                color: Theme.textOnSurfaceVariant
                Layout.alignment: Qt.AlignHCenter
            }
            Text {
                text: value
                font.family: Fonts.headlineFamily
                font.pixelSize: Fonts.largeTitle
                font.bold: true
                color: Theme.primary
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }
}
