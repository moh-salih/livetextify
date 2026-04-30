import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import LiveTextify
import "../singletons"

Rectangle {
    id: root

    // --- Pure UI Properties (Data flows downward) ---
    property string sessionTitle: "Meeting"
    property int sessionStatus: 0 // 0: Idle, 1: Recording, 2: Paused
    property int sessionSeconds: 0
    property string activeModelName: "Neural-v4-XL"
    property string modelTypeLabel: "MODEL:"
    property var availableModels: null

    // Helper for visual states
    readonly property bool isRecording: sessionStatus === 1

    // --- Pure UI Signals (Actions flow upward) ---
    signal titleEdited(string newTitle)
    signal configClicked()
    signal toggleRecordingClicked()
    signal endSessionClicked()
    signal modelSelected(int index)

    height: 120
    color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, 0.8)
    border.width: 1
    border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.1)

    function formatTime(s) {
        let hrs = Math.floor(s / 3600)
        let mins = Math.floor((s % 3600) / 60)
        let secs = s % 60
        return (hrs < 10 ? "0" : "") + hrs + ":" +
               (mins < 10 ? "0" : "") + mins + ":" +
               (secs < 10 ? "0" : "") + secs
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Top Row: Title & Badge
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 64

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 32
                anchors.rightMargin: 32
                spacing: 16

                // Editable Session Title
                TextField {
                    id: titleField
                    text: root.sessionTitle
                    font.family: Fonts.headlineFamily
                    font.pixelSize: Fonts.heading
                    font.bold: true
                    color: activeFocus ? Theme.primary : (titleHover.containsMouse ? Theme.primary : Theme.textOnSurface)
                    background: Item {}
                    selectByMouse: true
                    selectionColor: Theme.primary
                    selectedTextColor: Theme.background
                    Layout.maximumWidth: 400
                    Layout.preferredWidth: implicitWidth

                    Connections {
                        target: root
                        function onSessionTitleChanged() {
                            if (!titleField.activeFocus) {
                                titleField.text = root.sessionTitle
                            }
                        }
                    }

                    onAccepted: focus = false
                    onEditingFinished: {
                        if (text.trim() !== "" && text.trim() !== root.sessionTitle) {
                            root.titleEdited(text.trim())
                        } else {
                            text = root.sessionTitle // Revert if empty
                        }
                    }

                    MouseArea {
                        id: titleHover
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.IBeamCursor
                        acceptedButtons: Qt.NoButton // Pass clicks through
                    }
                }

                // Status Badge
                Rectangle {
                    Layout.preferredHeight: 28
                    Layout.preferredWidth: badgeRow.implicitWidth + 24
                    radius: 14
                    color: Qt.rgba(Theme.surfaceContainerLow.r, Theme.surfaceContainerLow.g, Theme.surfaceContainerLow.b, 0.4)
                    border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.1)
                    border.width: 1

                    RowLayout {
                        id: badgeRow
                        anchors.centerIn: parent
                        spacing: 8

                        Rectangle {
                            width: 8; height: 8; radius: 4
                            color: root.isRecording ? Theme.primary : Theme.outline
                            SequentialAnimation on opacity {
                                running: root.isRecording
                                loops: Animation.Infinite
                                NumberAnimation { to: 0.4; duration: 1000 }
                                NumberAnimation { to: 1.0; duration: 1000 }
                            }
                        }
                        Text {
                            text: root.sessionStatus === 1 ? "LIVE SESSION" : (root.sessionStatus === 2 ? "PAUSED" : "READY")
                            font.family: Fonts.bodyFamily
                            font.pixelSize: 10
                            font.bold: true
                            font.letterSpacing: 2
                            color: Theme.textOnSurfaceVariant
                        }
                    }
                }

                Item { Layout.fillWidth: true } // Visual balance spacer
            }
        }

        // Bottom Row: Unified Controls
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 56
            color: Qt.rgba(Theme.surfaceContainerLow.r, Theme.surfaceContainerLow.g, Theme.surfaceContainerLow.b, 0.3)
            border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.1)
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 32
                anchors.rightMargin: 32

                // Model Selection
                Rectangle {
                    id: modelSelectorContainer
                    Layout.preferredHeight: 36
                    Layout.preferredWidth: modelRow.implicitWidth + 24
                    radius: 8
                    color: modelHover.containsMouse || modelDropdown.opened ? Theme.surfaceContainerHigh : Theme.surfaceContainer
                    border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.1)
                    border.width: 1
                    Behavior on color { ColorAnimation { duration: 150 } }

                    RowLayout {
                        id: modelRow
                        anchors.centerIn: parent
                        spacing: 8
                        Text { text: root.modelTypeLabel; font.family: Fonts.bodyFamily; font.pixelSize: 10; font.bold: true; color: Theme.textOnSurfaceVariant }
                        Text { text: root.activeModelName; font.family: Fonts.bodyFamily; font.pixelSize: 12; font.bold: true; color: Theme.primary }
                        Text { text: "expand_more"; font.family: Fonts.iconFamily; font.pixelSize: 16; color: Theme.textOnSurfaceVariant }
                    }
                    MouseArea {
                        id: modelHover
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: modelDropdown.open()
                    }

                    // Model Dropdown Menu
                    Popup {
                        id: modelDropdown
                        y: parent.height + 8
                        x: 0
                        width: 280
                        padding: 8

                        // Evaluate downloaded models dynamically when opening to fix the placeholder logic
                        onAboutToShow: {
                            let hasAny = false;
                            for (let i = 0; i < modelRepeater.count; i++) {
                                let item = modelRepeater.itemAt(i);
                                if (item && item.isDownloaded) {
                                    hasAny = true;
                                    break;
                                }
                            }
                            placeholderItem.visible = !hasAny;
                        }

                        background: Rectangle {
                            color: Theme.surfaceContainerHighest
                            radius: 12
                            border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.3)
                            border.width: 1

                            layer.enabled: true
                            layer.effect: MultiEffect {
                                shadowEnabled: true
                                shadowColor: "#000000"
                                shadowBlur: 20
                            }
                        }

                        contentItem: ScrollView {
                            implicitHeight: Math.min(contentCol.implicitHeight, 250)
                            clip: true

                            ColumnLayout {
                                id: contentCol
                                width: parent ? parent.width : 0
                                spacing: 4

                                Repeater {
                                    id: modelRepeater
                                    model: root.availableModels
                                    delegate: Rectangle {
                                        property bool isDownloaded: model.downloaded === true

                                        // Only show downloaded models
                                        visible: isDownloaded

                                        Layout.fillWidth: true
                                        Layout.preferredHeight: visible ? 36 : 0
                                        radius: 8
                                        color: itemArea.containsMouse ? Qt.rgba(Theme.primary.r, Theme.primary.g, Theme.primary.b, 0.1) : "transparent"

                                        RowLayout {
                                            anchors.fill: parent
                                            anchors.margins: 8
                                            spacing: 8

                                            Text {
                                                text: model.fileName !== undefined ? model.fileName : "Unknown"
                                                font.family: Fonts.bodyFamily
                                                font.pixelSize: 12
                                                color: Theme.textOnSurface
                                                Layout.fillWidth: true
                                                elide: Text.ElideRight
                                            }

                                            Text {
                                                visible: root.activeModelName === (model.fileName !== undefined ? model.fileName : "")
                                                text: "check"
                                                font.family: Fonts.iconFamily
                                                font.pixelSize: 14
                                                color: Theme.primary
                                            }
                                        }

                                        MouseArea {
                                            id: itemArea
                                            anchors.fill: parent
                                            hoverEnabled: true
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: {
                                                root.modelSelected(index)
                                                modelDropdown.close()
                                            }
                                        }
                                    }
                                }

                                // Placeholder if no models are downloaded
                                Item {
                                    id: placeholderItem
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 36
                                    visible: false // Managed by Popup.onAboutToShow

                                    Text {
                                        anchors.centerIn: parent
                                        text: "No models downloaded"
                                        font.family: Fonts.bodyFamily
                                        font.pixelSize: 12
                                        color: Theme.textOnSurfaceVariant
                                        font.italic: true
                                    }
                                }
                            }
                        }
                    }
                }

                // Config Button
                Rectangle {
                    Layout.preferredHeight: 36
                    Layout.preferredWidth: 36
                    radius: 8
                    color: configHover.containsMouse ? Theme.surfaceContainerHigh : "transparent"
                    Text { anchors.centerIn: parent; text: "tune"; font.family: Fonts.iconFamily; font.pixelSize: 18; color: Theme.textOnSurfaceVariant }
                    MouseArea {
                        id: configHover
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: root.configClicked()
                    }
                }

                Item { Layout.fillWidth: true } // Center Spacer

                // Right-aligned Session Controls
                RowLayout {
                    spacing: 16

                    // Timer
                    Rectangle {
                        Layout.preferredHeight: 36
                        Layout.preferredWidth: timerRow.implicitWidth + 24
                        radius: 8
                        color: Qt.rgba(Theme.errorColor.r, Theme.errorColor.g, Theme.errorColor.b, 0.1)
                        border.color: Qt.rgba(Theme.errorColor.r, Theme.errorColor.g, Theme.errorColor.b, 0.2)
                        border.width: 1

                        RowLayout {
                            id: timerRow
                            anchors.centerIn: parent
                            spacing: 8
                            Rectangle {
                                width: 6; height: 6; radius: 3; color: Theme.errorColor
                                SequentialAnimation on opacity {
                                    running: root.isRecording
                                    loops: Animation.Infinite
                                    NumberAnimation { to: 0.2; duration: 800 }
                                    NumberAnimation { to: 1.0; duration: 800 }
                                }
                            }
                            Text {
                                text: root.formatTime(root.sessionSeconds)
                                font.family: Fonts.bodyFamily
                                font.pixelSize: 13
                                font.bold: true
                                font.letterSpacing: 1
                                color: Theme.errorColor
                            }
                        }
                    }

                    // Divider
                    Rectangle { Layout.preferredWidth: 1; Layout.preferredHeight: 24; color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.3) }

                    // Toggle Recording Button
                    Rectangle {
                        Layout.preferredHeight: 36
                        Layout.preferredWidth: toggleText.implicitWidth + 32
                        radius: 8
                        color: toggleBtnHover.containsMouse ? Theme.surfaceContainerHigh : "transparent"
                        Text {
                            id: toggleText
                            anchors.centerIn: parent
                            text: root.sessionStatus === 1 ? "Pause" : (root.sessionStatus === 2 ? "Resume" : "Start")
                            font.family: Fonts.bodyFamily
                            font.pixelSize: 13
                            font.bold: true
                            color: Theme.textOnSurfaceVariant
                        }
                        MouseArea {
                            id: toggleBtnHover
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: root.toggleRecordingClicked()
                        }
                    }

                    // End Session Button
                    Rectangle {
                        Layout.preferredHeight: 36
                        Layout.preferredWidth: endText.implicitWidth + 32
                        radius: 8
                        gradient: Theme.primaryGradient
                        Text {
                            id: endText
                            anchors.centerIn: parent
                            text: "End Session"
                            font.family: Fonts.bodyFamily
                            font.pixelSize: 13
                            font.bold: true
                            color: Theme.background
                        }
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onPressed: parent.scale = 0.95
                            onReleased: parent.scale = 1.0
                            onClicked: root.endSessionClicked()
                        }
                        Behavior on scale { NumberAnimation { duration: 100 } }
                    }
                }
            }
        }
    }
}
