import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import LiveTextify
import "../singletons"

Popup {
    id: root

    // The target C++ SessionSettings object to bind to
    property var settings

    // Track the active tab
    property int currentTab: 0

    signal reloadApproved()

    width: Math.min(760, Overlay.overlay ? Overlay.overlay.width - 48 : 760)
    height: Math.min(520, Overlay.overlay ? Overlay.overlay.height - 48 : 520)
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

    // --- Backend Signal Connections ---
    Connections {
        target: root.settings || null
        ignoreUnknownSignals: true

        // Listen for the backend telling us a heavy parameter changed
        function onReloadRequired() {
            reloadConfirmDialog.open()
        }
    }

    // --- Engine Reload Confirmation Dialog ---
    Popup {
        id: reloadConfirmDialog
        width: 400
        height: 220
        anchors.centerIn: parent
        modal: true
        dim: true
        closePolicy: Popup.NoAutoClose

        Overlay.modal: Rectangle { color: Qt.rgba(0, 0, 0, 0.6) }

        background: Rectangle {
            color: Theme.surfaceContainerHighest
            radius: 16
            border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.4)
            border.width: 1
            layer.enabled: true
            layer.effect: MultiEffect { shadowEnabled: true; shadowColor: "#000000"; shadowBlur: 30; shadowOpacity: 0.8 }
        }

        contentItem: ColumnLayout {
            spacing: 16

            RowLayout {
                spacing: 12
                Text { text: "warning"; font.family: Fonts.iconFamily; font.pixelSize: 28; color: Theme.errorColor }
                Text { text: "Engine Reload Required"; font.family: Fonts.headlineFamily; font.pixelSize: 18; font.bold: true; color: Theme.textOnSurface }
            }

            Text {
                text: "Applying this setting requires the neural engine to restart. Active generation or transcription will be momentarily interrupted."
                font.family: Fonts.bodyFamily
                font.pixelSize: 13
                color: Theme.textOnSurfaceVariant
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                lineHeight: 1.4
            }

            Item { Layout.fillHeight: true } // Spacer

            RowLayout {
                Layout.fillWidth: true
                spacing: 16

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 44
                    radius: 8
                    color: cancelHover.containsMouse ? Qt.rgba(255, 255, 255, 0.05) : "transparent"
                    border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.6)
                    border.width: 1
                    Text { anchors.centerIn: parent; text: "Later"; font.family: Fonts.bodyFamily; font.pixelSize: 13; font.bold: true; color: Theme.textOnSurfaceVariant }
                    MouseArea {
                        id: cancelHover
                        anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor
                        onClicked: reloadConfirmDialog.close()
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 44
                    radius: 8
                    color: reloadHover.containsMouse ? Qt.rgba(Theme.errorColor.r, Theme.errorColor.g, Theme.errorColor.b, 0.8) : Theme.errorColor
                    Behavior on color { ColorAnimation { duration: 150 } }

                    Text { anchors.centerIn: parent; text: "Reload Now"; font.family: Fonts.bodyFamily; font.pixelSize: 13; font.bold: true; color: Theme.background }
                    MouseArea {
                        id: reloadHover
                        anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor
                        onPressed: parent.scale = 0.97
                        onReleased: parent.scale = 1.0
                        onClicked: {
                            // Call the C++ invokable you created
                            if (root.settings) {
                                root.settings.approveReload()
                            }
                            reloadConfirmDialog.close()
                        }
                    }
                    Behavior on scale { NumberAnimation { duration: 100 } }
                }
            }
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
                    Text { text: "Session Configuration"; font.family: Fonts.headlineFamily; font.pixelSize: 18; font.bold: true; color: Theme.textOnSurface }
                    Text { text: "Adjust inference, context, and memory parameters."; font.family: Fonts.bodyFamily; font.pixelSize: 11; color: Theme.textOnSurfaceVariant }
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

        // --- Custom Tab Bar ---
        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 24
            Layout.rightMargin: 24
            Layout.topMargin: 16
            Layout.bottomMargin: 8
            spacing: 12

            TabButton { text: "Speech (STT)"; icon: "mic"; tabIndex: 0; activeColor: Theme.primary }
            TabButton { text: "Language (LLM)"; icon: "neurology"; tabIndex: 1; activeColor: Theme.secondary }
            TabButton { text: "Memory (RAG)"; icon: "hub"; tabIndex: 2; activeColor: Theme.tertiary }
            TabButton { text: "Audio Engine"; icon: "graphic_eq"; tabIndex: 3; activeColor: Theme.errorColor }

            Item { Layout.fillWidth: true } // Push tabs to the left
        }

        // --- Tab Content Area ---
        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 24
            currentIndex: root.currentTab

            // ── Tab 0: STT ───────────────────────────────────────────────────────
            ColumnLayout {
                spacing: 24

                GridLayout {
                    Layout.fillWidth: true
                    columns: 2
                    columnSpacing: 32
                    rowSpacing: 16

                    SettingsInput {
                        Layout.fillWidth: true
                        label: "Language Code"
                        hint: "e.g., 'en', 'fr'"
                        text: root.settings ? root.settings.language : "en"
                        onTextChanged: {
                            if (root.settings && text !== root.settings.language) {
                                root.settings.language = text;
                            }
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        RowLayout {
                            Layout.fillWidth: true
                            Text { text: "Auto Detect Language"; font.family: Fonts.headlineFamily; font.pixelSize: 13; font.bold: true; color: Theme.textOnSurface; Layout.fillWidth: true }
                            SettingsToggle {
                                checked: root.settings ? root.settings.autoDetectLanguage : false
                                onCheckedChanged: if (root.settings) root.settings.autoDetectLanguage = checked
                                themeColor: Theme.primary
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            Text { text: "Translate to English"; font.family: Fonts.headlineFamily; font.pixelSize: 13; font.bold: true; color: Theme.textOnSurface; Layout.fillWidth: true }
                            SettingsToggle {
                                checked: root.settings ? root.settings.translate : false
                                onCheckedChanged: if (root.settings) root.settings.translate = checked
                                themeColor: Theme.primary
                            }
                        }
                    }
                }
                Item { Layout.fillHeight: true } // Push content up
            }

            // ── Tab 1: LLM ───────────────────────────────────────────────────────
            ColumnLayout {
                spacing: 16

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4
                    Text { text: "System Prompt"; font.family: Fonts.headlineFamily; font.pixelSize: 13; font.bold: true; color: Theme.textOnSurface }
                    SettingsTextArea {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 52
                        text: root.settings ? root.settings.systemPrompt : ""
                        onTextChanged: if (root.settings && text !== root.settings.systemPrompt) root.settings.systemPrompt = text
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    Text { text: "memory"; font.family: Fonts.iconFamily; font.pixelSize: 15; color: Theme.secondary }
                    Text {
                        text: "Resolved Context Length: " + (root.settings ? root.settings.resolvedContextLength : "0") + " tokens"
                        font.family: Fonts.bodyFamily; font.pixelSize: 11; font.bold: true; color: Theme.textOnSurfaceVariant
                    }
                }

                Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.2) }

                GridLayout {
                    Layout.fillWidth: true
                    columns: 2
                    columnSpacing: 32
                    rowSpacing: 12

                    SettingsSlider {
                        Layout.fillWidth: true
                        label: "Context Length"
                        valueText: (root.settings && root.settings.llmContextLength === 0) ? "Auto (Full)" : (root.settings ? Number(root.settings.llmContextLength).toString() : "0")
                        from: 0; to: 32768; stepSize: 512
                        value: root.settings ? root.settings.llmContextLength : 0
                        onValueChanged: if (root.settings) root.settings.llmContextLength = value
                        themeColor: Theme.secondary
                    }

                    SettingsSlider {
                        Layout.fillWidth: true
                        label: "Max Tokens"
                        valueText: root.settings ? Number(root.settings.maxTokens).toString() : "256"
                        from: 16; to: 4096; stepSize: 16
                        value: root.settings ? root.settings.maxTokens : 256
                        onValueChanged: if (root.settings) root.settings.maxTokens = value
                        themeColor: Theme.secondary
                    }

                    SettingsSlider {
                        Layout.fillWidth: true
                        label: "Temperature"
                        valueText: root.settings ? Number(root.settings.temperature).toFixed(2) : "0.70"
                        from: 0.0; to: 2.0; stepSize: 0.05
                        value: root.settings ? root.settings.temperature : 0.7
                        onValueChanged: if (root.settings) root.settings.temperature = value
                        themeColor: Theme.secondary
                    }

                    SettingsSlider {
                        Layout.fillWidth: true
                        label: "Top P"
                        valueText: root.settings ? Number(root.settings.topP).toFixed(2) : "0.90"
                        from: 0.0; to: 1.0; stepSize: 0.05
                        value: root.settings ? root.settings.topP : 0.9
                        onValueChanged: if (root.settings) root.settings.topP = value
                        themeColor: Theme.secondary
                    }

                    SettingsSlider {
                        Layout.fillWidth: true
                        label: "Top K"
                        valueText: root.settings ? Number(root.settings.topK).toString() : "40"
                        from: 1; to: 100; stepSize: 1
                        value: root.settings ? root.settings.topK : 40
                        onValueChanged: if (root.settings) root.settings.topK = value
                        themeColor: Theme.secondary
                    }

                    SettingsSlider {
                        Layout.fillWidth: true
                        label: "Repeat Penalty"
                        valueText: root.settings ? Number(root.settings.repeatPenalty).toFixed(2) : "1.10"
                        from: 1.0; to: 2.0; stepSize: 0.05
                        value: root.settings ? root.settings.repeatPenalty : 1.1
                        onValueChanged: if (root.settings) root.settings.repeatPenalty = value
                        themeColor: Theme.secondary
                    }
                }
                Item { Layout.fillHeight: true } // Push content up
            }

            // ── Tab 2: RAG ───────────────────────────────────────────────────────
            ColumnLayout {
                spacing: 16

                RowLayout {
                    Layout.fillWidth: true
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2
                        Text { text: "Enable RAG Context"; font.family: Fonts.headlineFamily; font.pixelSize: 13; font.bold: true; color: Theme.textOnSurface }
                        Text { text: "Feed relevant transcription history into the LLM chat automatically."; font.family: Fonts.bodyFamily; font.pixelSize: 11; color: Theme.textOnSurfaceVariant }
                    }
                    SettingsToggle {
                        checked: root.settings ? root.settings.enableRag : true
                        onCheckedChanged: if (root.settings) root.settings.enableRag = checked
                        themeColor: Theme.tertiary
                    }
                }

                Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.2) }

                GridLayout {
                    Layout.fillWidth: true
                    columns: 2
                    columnSpacing: 32
                    rowSpacing: 16

                    SettingsSlider {
                        Layout.fillWidth: true
                        label: "Top K Results"
                        valueText: root.settings ? Number(root.settings.ragTopK).toString() : "5"
                        from: 1; to: 20; stepSize: 1
                        value: root.settings ? root.settings.ragTopK : 5
                        onValueChanged: if (root.settings) root.settings.ragTopK = value
                        themeColor: Theme.tertiary
                    }

                    SettingsSlider {
                        Layout.fillWidth: true
                        label: "Min Score (Similarity)"
                        valueText: root.settings ? Number(root.settings.ragMinScore).toFixed(2) : "0.00"
                        from: 0.0; to: 1.0; stepSize: 0.05
                        value: root.settings ? root.settings.ragMinScore : 0.0
                        onValueChanged: if (root.settings) root.settings.ragMinScore = value
                        themeColor: Theme.tertiary
                    }

                    SettingsInput {
                        Layout.fillWidth: true
                        label: "Min Chunk Size"
                        text: root.settings ? root.settings.minChunkSize.toString() : "40"
                        onTextChanged: {
                            if (root.settings && text !== "") {
                                let val = parseInt(text);
                                if (!isNaN(val)) root.settings.minChunkSize = val;
                            }
                        }
                    }

                    SettingsInput {
                        Layout.fillWidth: true
                        label: "Max Chunk Size"
                        text: root.settings ? root.settings.maxChunkSize.toString() : "400"
                        onTextChanged: {
                            if (root.settings && text !== "") {
                                let val = parseInt(text);
                                if (!isNaN(val)) root.settings.maxChunkSize = val;
                            }
                        }
                    }
                }
                Item { Layout.fillHeight: true } // Push content up
            }

            // ── Tab 3: Audio ─────────────────────────────────────────────────────
            ColumnLayout {
                spacing: 24

                GridLayout {
                    Layout.fillWidth: true
                    columns: 2
                    columnSpacing: 32
                    rowSpacing: 16

                    SettingsInput {
                        Layout.fillWidth: true
                        label: "Max Window (ms)"
                        hint: "Maximum size of the sliding audio window."
                        text: root.settings ? root.settings.maxWindow.toString() : "5000"
                        onTextChanged: {
                            if (root.settings && text !== "") {
                                let val = parseInt(text);
                                if (!isNaN(val)) root.settings.maxWindow = val;
                            }
                        }
                    }

                    SettingsInput {
                        Layout.fillWidth: true
                        label: "Step Duration (ms)"
                        hint: "How often to advance the sliding window."
                        text: root.settings ? root.settings.step.toString() : "4500"
                        onTextChanged: {
                            if (root.settings && text !== "") {
                                let val = parseInt(text);
                                if (!isNaN(val)) root.settings.step = val;
                            }
                        }
                    }

                    SettingsInput {
                        Layout.fillWidth: true
                        label: "Keep Duration (ms)"
                        hint: "Overlap to keep from the previous window."
                        text: root.settings ? root.settings.keep.toString() : "500"
                        onTextChanged: {
                            if (root.settings && text !== "") {
                                let val = parseInt(text);
                                if (!isNaN(val)) root.settings.keep = val;
                            }
                        }
                    }
                }
                Item { Layout.fillHeight: true } // Push content up
            }
        }
    }

    // --- Custom Tab Button Component ---
    component TabButton: Rectangle {
        property string text
        property string icon
        property int tabIndex
        property color activeColor
        property bool isActive: root.currentTab === tabIndex

        Layout.preferredHeight: 36
        Layout.preferredWidth: tabContentRow.implicitWidth + 24
        radius: 8

        color: isActive ? Qt.rgba(activeColor.r, activeColor.g, activeColor.b, 0.15) : (tabMouseArea.containsMouse ? Theme.surfaceContainerHigh : "transparent")
        border.color: isActive ? activeColor : "transparent"
        border.width: 1

        Behavior on color { ColorAnimation { duration: 150 } }
        Behavior on border.color { ColorAnimation { duration: 150 } }

        RowLayout {
            id: tabContentRow
            anchors.centerIn: parent
            spacing: 8

            Text {
                text: parent.parent.icon
                font.family: Fonts.iconFamily
                font.pixelSize: 16
                color: parent.parent.isActive ? parent.parent.activeColor : Theme.textOnSurfaceVariant
            }
            Text {
                text: parent.parent.text
                font.family: Fonts.bodyFamily
                font.pixelSize: 12
                font.bold: true
                color: parent.parent.isActive ? parent.parent.activeColor : Theme.textOnSurfaceVariant
            }
        }

        MouseArea {
            id: tabMouseArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: root.currentTab = parent.tabIndex
        }
    }
}
