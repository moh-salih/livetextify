import QtQuick
import QtQuick.Layouts
import QtQuick.Effects
import LiveTextify
import "../singletons"

Rectangle {
    id: root

    // ── Data properties — bound by parent ─────────────────────────────────
    property real   ramUsed  : 0.0   // in GB
    property real   ramTotal : 16.0  // in GB (Mock default)
    property real   cpuUsage : 0.0   // 0–100
    property int    threads  : 0
    property bool   hasGpu   : false
    property string gpuName  : "NVIDIA RTX 4090"
    property real   gpuUsed  : 0.0   // in GB
    property real   gpuTotal : 8.0   // in GB (Mock default)

    Layout.fillHeight: true
    Layout.preferredWidth: 140 // Much more compact width
    color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, 0.8)

    // Right subtle border
    Rectangle {
        width: 1
        height: parent.height
        anchors.right: parent.right
        color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.2)
    }

    Flickable {
        anchors.fill: parent
        contentWidth: width
        contentHeight: contentCol.implicitHeight + 32
        boundsBehavior: Flickable.StopAtBounds
        clip: true

        // Hide scrollbar but allow panning if screen is extremely short
        interactive: contentHeight > height

        ColumnLayout {
            id: contentCol
            width: parent.width
            anchors.top: parent.top
            anchors.topMargin: 16
            spacing: 16

            // 1. RAM METRIC
            ColumnLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16
                spacing: 4

                Text {
                    text: "RAM"
                    font.family: Fonts.headlineFamily
                    font.pixelSize: 9
                    font.bold: true
                    color: Theme.primary
                    font.letterSpacing: 1
                }

                Text {
                    text: root.ramUsed.toFixed(1) + " GB / " + root.ramTotal.toFixed(1) + " GB"
                    font.family: Fonts.headlineFamily
                    font.pixelSize: 10
                    font.weight: Font.Medium
                    color: Theme.textOnSurface
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 4
                    Layout.topMargin: 2
                    radius: 2
                    color: Theme.surfaceContainerLowest

                    Rectangle {
                        width: root.ramTotal > 0 ? parent.width * (root.ramUsed / root.ramTotal) : 0
                        height: parent.height
                        radius: 2
                        gradient: Theme.primaryGradient
                    }
                }
            }

            // Divider
            Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.2) }

            // 2. CPU METRIC
            ColumnLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16
                spacing: 4

                Text {
                    text: "CPU"
                    font.family: Fonts.headlineFamily
                    font.pixelSize: 9
                    font.bold: true
                    color: Theme.primary
                    font.letterSpacing: 1
                }

                Text {
                    text: root.cpuUsage.toFixed(1) + "%"
                    font.family: Fonts.headlineFamily
                    font.pixelSize: 10
                    font.weight: Font.Medium
                    color: Theme.textOnSurface
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 4
                    Layout.topMargin: 2
                    radius: 2
                    color: Theme.surfaceContainerLowest

                    Rectangle {
                        width: parent.width * (root.cpuUsage / 100.0)
                        height: parent.height
                        radius: 2
                        color: Theme.primary
                        SequentialAnimation on opacity {
                            loops: Animation.Infinite
                            NumberAnimation { to: 0.6; duration: 1000; easing.type: Easing.InOutQuad }
                            NumberAnimation { to: 1.0; duration: 1000; easing.type: Easing.InOutQuad }
                        }
                    }
                }
            }

            // Divider
            Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.2) }

            // 3. THREADS METRIC
            ColumnLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16
                spacing: 4

                Text {
                    text: "THREADS"
                    font.family: Fonts.headlineFamily
                    font.pixelSize: 9
                    font.bold: true
                    color: Theme.textOnSurfaceVariant
                    font.letterSpacing: 1
                }

                RowLayout {
                    spacing: 6
                    Text { text: "psychology"; font.family: Fonts.iconFamily; font.pixelSize: 14; color: Theme.primary }
                    Text { text: root.threads.toString(); font.family: Fonts.headlineFamily; font.pixelSize: 10; font.bold: true; color: Theme.textOnSurface }
                }
            }

            // Divider
            Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.2) }

            // 4. GPU METRIC
            ColumnLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16
                spacing: 4

                Text {
                    text: root.gpuName
                    font.family: Fonts.headlineFamily
                    font.pixelSize: 9
                    font.bold: true
                    color: Theme.secondary
                    font.letterSpacing: 1
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }

                Text {
                    text: root.gpuUsed.toFixed(1) + " GB / " + root.gpuTotal.toFixed(1) + " GB"
                    font.family: Fonts.headlineFamily
                    font.pixelSize: 10
                    font.weight: Font.Medium
                    color: Theme.textOnSurface
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 4
                    Layout.topMargin: 2
                    radius: 2
                    color: Theme.surfaceContainerLowest

                    Rectangle {
                        width: root.gpuTotal > 0 ? parent.width * (root.gpuUsed / root.gpuTotal) : 0
                        height: parent.height
                        radius: 2
                        gradient: Gradient {
                            orientation: Gradient.Horizontal
                            GradientStop { position: 0.0; color: Theme.secondary }
                            GradientStop { position: 1.0; color: Theme.tertiary }
                        }
                    }
                }
            }

            // Divider
            Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.2) }

            // 5. STATUS INDICATOR
            RowLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16
                spacing: 6

                Rectangle {
                    width: 6; height: 6; radius: 3; color: Theme.primary
                    SequentialAnimation on opacity {
                        loops: Animation.Infinite
                        NumberAnimation { to: 0.3; duration: 1500; easing.type: Easing.InOutQuad }
                        NumberAnimation { to: 1.0; duration: 1500; easing.type: Easing.InOutQuad }
                    }
                }
                Text {
                    text: "SYSTEM_ALIVE"
                    font.family: Fonts.headlineFamily
                    font.pixelSize: 8
                    font.bold: true
                    color: Theme.primary
                    font.letterSpacing: 1.5
                }
            }
        }
    }
}
