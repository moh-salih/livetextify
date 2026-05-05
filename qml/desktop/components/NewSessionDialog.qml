import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import LiveTextify
import "../singletons"

Popup {
    id: root

    property var settings
    property string defaultTitle: "New Session"

    // Component Signals (Upward to Mediator)
    signal startRequested(string title)
    signal resetDefaultsRequested()

    width: Math.min(760, Overlay.overlay ? Overlay.overlay.width - 48 : 760)
    height: Math.min(680, Overlay.overlay ? Overlay.overlay.height - 48 : 680)
    anchors.centerIn: Overlay.overlay
    modal: true
    dim: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    // --- Animations ---
    enter: Transition {
        ParallelAnimation {
            NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 200; easing.type: Easing.OutCubic }
            NumberAnimation { property: "scale"; from: 0.95; to: 1.0; duration: 200; easing.type: Easing.OutBack }
        }
    }
    exit: Transition {
        ParallelAnimation {
            NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 150; easing.type: Easing.InCubic }
            NumberAnimation { property: "scale"; from: 1.0; to: 0.95; duration: 150; easing.type: Easing.InCubic }
        }
    }

    Overlay.modal: Rectangle {
        color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, 0.7)
        layer.enabled: true
        layer.effect: MultiEffect { blurEnabled: true; blurMax: 24; blur: 1.0 }
    }

    background: Rectangle {
        color: Theme.surfaceContainer
        radius: 16
        border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.4)
        border.width: 1

        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true; shadowColor: "#000000"
            shadowOpacity: 0.5; shadowBlur: 30; shadowVerticalOffset: 8
        }
    }

    contentItem: ColumnLayout {
        width: root.availableWidth
        height: root.availableHeight
        spacing: 0

        // --- Header ---
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 64
            color: "transparent"

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 24
                anchors.rightMargin: 16

                ColumnLayout {
                    spacing: 2
                    Text { text: "New Transcription Session"; font.family: Fonts.headlineFamily; font.pixelSize: 18; font.bold: true; color: Theme.textOnSurface }
                    Text { text: "Configure settings and start capturing live audio."; font.family: Fonts.bodyFamily; font.pixelSize: 11; color: Theme.textOnSurfaceVariant }
                }

                Item { Layout.fillWidth: true }

                Rectangle {
                    Layout.preferredWidth: 32
                    Layout.preferredHeight: 32
                    radius: 16
                    color: closeHover.containsMouse ? Theme.surfaceContainerHigh : "transparent"
                    Text { anchors.centerIn: parent; text: "close"; font.family: Fonts.iconFamily; font.pixelSize: 18; color: Theme.textOnSurfaceVariant }
                    MouseArea { id: closeHover; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: root.close() }
                }
            }

            Rectangle { width: parent.width; height: 1; anchors.bottom: parent.bottom; color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.2) }
        }

        // --- Body ---
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            // Title Input Block
            ColumnLayout {
                Layout.fillWidth: true
                Layout.margins: 24
                Layout.bottomMargin: 16
                spacing: 8

                Text { text: "Session Title"; font.family: Fonts.headlineFamily; font.pixelSize: 12; font.bold: true; color: Theme.textOnSurface }

                TextField {
                    id: titleField
                    text: root.defaultTitle
                    font.family: Fonts.bodyFamily
                    font.pixelSize: 14
                    color: Theme.textOnSurface
                    background: Rectangle {
                        color: Theme.surfaceContainerLowest
                        radius: 8
                        border.color: titleField.activeFocus ? Theme.primary : Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.3)
                    }
                    Layout.fillWidth: true
                    Layout.preferredHeight: 44
                    leftPadding: 16
                    rightPadding: 16
                }
            }

            Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.2) }

            // Reusable Settings View
            SessionSettingsView {
                id: settingsView
                settings: root.settings
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }

        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.2) }

        // --- Footer (Actions) ---
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 64
            color: Qt.rgba(Theme.surfaceContainerLow.r, Theme.surfaceContainerLow.g, Theme.surfaceContainerLow.b, 0.5)
            radius: 16 // Covers the bottom corners

            RowLayout {
                anchors.fill: parent
                anchors.rightMargin: 24
                anchors.leftMargin: 24
                spacing: 16

                // Reset Button (Flows action UPWARD to mediator)
                Rectangle {
                    Layout.preferredWidth: resetRow.implicitWidth + 24
                    Layout.preferredHeight: 40
                    radius: 8
                    color: resetArea.containsMouse ? Qt.rgba(Theme.errorColor.r, Theme.errorColor.g, Theme.errorColor.b, 0.15) : "transparent"
                    border.color: resetArea.containsMouse ? Theme.errorColor : "transparent"
                    border.width: 1

                    RowLayout {
                        id: resetRow
                        anchors.centerIn: parent
                        spacing: 6
                        Text { text: "refresh"; font.family: Fonts.iconFamily; font.pixelSize: 14; color: Theme.errorColor }
                        Text { text: "Reset Defaults"; font.family: Fonts.bodyFamily; font.pixelSize: 12; font.bold: true; color: Theme.errorColor }
                    }

                    MouseArea {
                        id: resetArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: root.resetDefaultsRequested()
                    }
                    Behavior on color { ColorAnimation { duration: 150 } }
                    Behavior on border.color { ColorAnimation { duration: 150 } }
                }

                Item { Layout.fillWidth: true } // push right

                Rectangle {
                    Layout.preferredWidth: 100
                    Layout.preferredHeight: 40
                    radius: 8
                    color: cancelBtnArea.containsMouse ? Qt.rgba(255, 255, 255, 0.05) : "transparent"
                    border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.6)
                    border.width: 1
                    Text { anchors.centerIn: parent; text: "Cancel"; font.family: Fonts.bodyFamily; font.pixelSize: 13; font.bold: true; color: Theme.textOnSurfaceVariant }
                    MouseArea {
                        id: cancelBtnArea
                        anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor
                        onClicked: root.close()
                    }
                }

                Rectangle {
                    Layout.preferredWidth: 160
                    Layout.preferredHeight: 40
                    radius: 8
                    gradient: Theme.primaryGradient
                    RowLayout {
                        anchors.centerIn: parent
                        spacing: 8
                        Text { text: "play_arrow"; font.family: Fonts.iconFamily; font.pixelSize: 18; color: Theme.background }
                        Text { text: "Start Session"; font.family: Fonts.bodyFamily; font.pixelSize: 13; font.bold: true; color: Theme.background }
                    }
                    MouseArea {
                        anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor
                        onPressed: parent.scale = 0.96
                        onReleased: parent.scale = 1.0
                        onClicked: {
                            root.startRequested(titleField.text)
                        }
                        Behavior on scale { NumberAnimation { duration: 100 } }
                    }
                }
            }
        }
    }
}
