import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify
import "../singletons"

Rectangle {
    id: root

    // UI Properties
    property bool isRecording: false
    property string engineName: "Neural-V4"
    property string contextUsage: "12 / 128k"
    property real contextProgress: 0.09

    // Status Properties
    property int sttStatus: LiveTextify.EngineStatus.Idle
    property int llmStatus: LiveTextify.EngineStatus.Idle
    property int ragStatus: LiveTextify.EngineStatus.Idle

    // Signals
    signal toggleRecordingClicked()
    signal copyTranscriptClicked()
    signal endSessionClicked()

    color: Qt.rgba(Theme.surfaceContainerLow.r, Theme.surfaceContainerLow.g, Theme.surfaceContainerLow.b, 0.4)

    // Left border line
    Rectangle {
        width: 1; height: parent.height
        anchors.left: parent.left
        color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.1)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 24

        // Top Control: Record / Pause Button
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            radius: 16
            color: root.isRecording ? Theme.primary : Qt.rgba(Theme.primary.r, Theme.primary.g, Theme.primary.b, 0.1)
            border.color: Qt.rgba(Theme.primary.r, Theme.primary.g, Theme.primary.b, 0.2)

            ColumnLayout {
                anchors.centerIn: parent
                spacing: 12

                Text {
                    text: root.isRecording ? "pause_circle" : "radio_button_checked"
                    font.family: Fonts.iconFamily
                    font.pixelSize: 36
                    color: root.isRecording ? Theme.textOnPrimary : Theme.primary
                    Layout.alignment: Qt.AlignHCenter
                }

                Text {
                    text: root.isRecording ? "PAUSE RECORDING" : "START RECORDING"
                    font.family: Fonts.headlineFamily
                    font.pixelSize: 10
                    font.bold: true
                    font.letterSpacing: 2
                    color: root.isRecording ? Theme.textOnPrimary : Theme.primary
                    Layout.alignment: Qt.AlignHCenter
                }
            }

            MouseArea {
                id: recordHoverArea
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                hoverEnabled: true
                onPressed: parent.scale = 0.98
                onReleased: parent.scale = 1.0
                onClicked: root.toggleRecordingClicked()
            }
            Behavior on scale { NumberAnimation { duration: 100 } }
        }

        // Engine Status Block
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 56
            radius: 16
            color: Qt.rgba(Theme.surfaceContainerHighest.r, Theme.surfaceContainerHighest.g, Theme.surfaceContainerHighest.b, 0.3)
            border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.1)

            RowLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 0

                EngineStatusBadge {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    activeColor: Theme.primary
                    labelText: "STT"
                    status: root.sttStatus
                }

                Rectangle {
                    width: 1; Layout.fillHeight: true
                    color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.1)
                    Layout.topMargin: 12; Layout.bottomMargin: 12
                }

                EngineStatusBadge {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    activeColor: Theme.secondary
                    labelText: "LLM"
                    status: root.llmStatus
                }

                Rectangle {
                    width: 1; Layout.fillHeight: true
                    color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.1)
                    Layout.topMargin: 12; Layout.bottomMargin: 12
                }

                EngineStatusBadge {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    activeColor: Theme.tertiary
                    labelText: "RAG"
                    status: root.ragStatus
                }
            }
        }

        // Model Details Block
        Rectangle {
            visible: false
            Layout.fillWidth: true
            Layout.preferredHeight: 130
            radius: 16
            color: Qt.rgba(Theme.surfaceContainerHighest.r, Theme.surfaceContainerHighest.g, Theme.surfaceContainerHighest.b, 0.2)
            border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.05)

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 16

                Text {
                    text: "NEURAL INFRASTRUCTURE"
                    font.family: Fonts.headlineFamily
                    font.pixelSize: 9
                    font.bold: true
                    font.letterSpacing: 2
                    color: Theme.textOnSurfaceVariant
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: "ENGINE"; font.family: Fonts.bodyFamily; font.pixelSize: 10; font.bold: true; font.letterSpacing: 1; color: Theme.textOnSurfaceVariant; opacity: 0.5 }
                        Item { Layout.fillWidth: true }
                        Text { text: root.engineName; font.family: Fonts.bodyFamily; font.pixelSize: 10; font.bold: true; color: Theme.primary }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: "CONTEXT"; font.family: Fonts.bodyFamily; font.pixelSize: 10; font.bold: true; font.letterSpacing: 1; color: Theme.textOnSurfaceVariant; opacity: 0.5 }
                        Item { Layout.fillWidth: true }
                        Text { text: root.contextUsage; font.family: Fonts.bodyFamily; font.pixelSize: 10; font.bold: true; color: Theme.textOnSurface }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 4
                        radius: 2
                        color: Theme.surfaceContainerLow
                        Rectangle {
                            width: parent.width * root.contextProgress
                            height: parent.height
                            radius: 2
                            color: Theme.primary
                        }
                    }
                }
            }
        }

        // Utilities: Copy Button
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 52
            radius: 16
            color: copyHoverArea.containsMouse ? Theme.surfaceContainerHighest : Qt.rgba(Theme.surfaceContainerHigh.r, Theme.surfaceContainerHigh.g, Theme.surfaceContainerHigh.b, 0.5)
            border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.1)

            Behavior on color { ColorAnimation { duration: 150 } }

            RowLayout {
                anchors.centerIn: parent
                spacing: 12
                Text { text: "content_copy"; font.family: Fonts.iconFamily; font.pixelSize: 20; color: Theme.textOnSurface }
                Text { text: "COPY TRANSCRIPT"; font.family: Fonts.headlineFamily; font.pixelSize: 10; font.bold: true; font.letterSpacing: 1.5; color: Theme.textOnSurface }
            }

            MouseArea {
                id: copyHoverArea
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                hoverEnabled: true
                onClicked: root.copyTranscriptClicked()
            }
        }

        Item { Layout.fillHeight: true }

        // Bottom Action: End Session
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 64
            radius: 16
            color: endSessionHover.containsMouse ? "#D32F2F" : "#E53935"

            // Dark subtle shadow
            Rectangle {
                anchors.fill: parent
                radius: 16
                color: "transparent"
                border.color: Qt.rgba(0,0,0,0.5)
                z: -1
            }

            RowLayout {
                anchors.centerIn: parent
                spacing: 12
                Text { text: "cancel"; font.family: Fonts.iconFamily; font.pixelSize: 20; color: "white" }
                Text { text: "END SESSION"; font.family: Fonts.headlineFamily; font.pixelSize: 10; font.bold: true; font.letterSpacing: 2; color: "white" }
            }

            MouseArea {
                id: endSessionHover
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                hoverEnabled: true
                onPressed: parent.scale = 0.98
                onReleased: parent.scale = 1.0
                onClicked: root.endSessionClicked()
            }
            Behavior on scale { NumberAnimation { duration: 100 } }
            Behavior on color { ColorAnimation { duration: 150 } }
        }
    }

    component EngineStatusBadge: Item {
        id: badgeRoot
        property color activeColor
        property string labelText
        property int status: LiveTextify.EngineStatus.Idle

        property color dotColor: {
            if (status === LiveTextify.EngineStatus.Error) return "#ff716c"
            if (status === LiveTextify.EngineStatus.Ready || status === LiveTextify.EngineStatus.Loading) return activeColor
            return Theme.outline
        }

        RowLayout {
            anchors.centerIn: parent
            spacing: 6

            Rectangle {
                id: dotIndicator
                width: 6; height: 6; radius: 3
                color: badgeRoot.dotColor

                SequentialAnimation {
                    running: badgeRoot.status === LiveTextify.EngineStatus.Ready || badgeRoot.status === LiveTextify.EngineStatus.Loading
                    loops: Animation.Infinite

                    NumberAnimation { target: dotIndicator; property: "opacity"; to: 0.2; duration: 800; easing.type: Easing.InOutQuad }
                    NumberAnimation { target: dotIndicator; property: "opacity"; to: 1.0; duration: 800; easing.type: Easing.InOutQuad }
                }
                Connections {
                    target: badgeRoot
                    function onStatusChanged() {
                        if (badgeRoot.status === LiveTextify.EngineStatus.Idle || badgeRoot.status === LiveTextify.EngineStatus.Error) {
                            dotIndicator.opacity = 1.0
                        }
                    }
                }
            }

            Text {
                text: badgeRoot.labelText
                font.family: Fonts.bodyFamily
                font.pixelSize: 9
                font.bold: true
                font.letterSpacing: 1
                color: badgeRoot.status !== LiveTextify.EngineStatus.Idle ? badgeRoot.dotColor : Theme.textOnSurfaceVariant
            }
        }
    }
}
