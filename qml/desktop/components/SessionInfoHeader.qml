import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify
import "../singletons"

Rectangle {
    id: root

    // --- Pure UI Properties ---
    property string sessionTitle: "Strategy Alignment Q3"
    property string startTimeText: "Started 14:02"

    signal titleEdited(string newTitle)

    height: 80
    color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, 0.3)

    // Bottom border
    Rectangle {
        width: parent.width; height: 1
        anchors.bottom: parent.bottom
        color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.05)
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 32
        anchors.rightMargin: 32
        spacing: 24

        // Title and Subtitles
        RowLayout {
            spacing: 16

            TextField {
                id: titleField
                text: root.sessionTitle
                font.family: Fonts.headlineFamily
                font.pixelSize: Fonts.heading
                font.bold: true
                color: activeFocus ? Theme.primary : (titleHover.containsMouse ? Theme.primary : Theme.textOnSurface)

                Layout.maximumWidth: 400
                Layout.preferredWidth: implicitWidth

                background: Item {} // Transparent background
                leftPadding: 0
                rightPadding: (titleHover.containsMouse && !activeFocus) ? 28 : 0

                selectByMouse: true
                selectionColor: Theme.primary
                selectedTextColor: Theme.background

                // Re-sync with backend if title changes externally
                Connections {
                    target: root
                    function onSessionTitleChanged() {
                        if (!titleField.activeFocus) {
                            titleField.text = root.sessionTitle
                        }
                    }
                }

                onAccepted: {
                    focus = false // Defocusing triggers onEditingFinished
                }

                onEditingFinished: {
                    if (text.trim() !== "" && text.trim() !== root.sessionTitle) {
                        root.titleEdited(text.trim())
                    } else {
                        text = root.sessionTitle // Revert if empty
                    }
                }

                Behavior on color { ColorAnimation { duration: 150 } }

                MouseArea {
                    id: titleHover
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.IBeamCursor
                    acceptedButtons: Qt.NoButton // Pass clicks through to the TextField
                }

                Text {
                    text: "edit"
                    font.family: Fonts.iconFamily
                    font.pixelSize: 18
                    color: Theme.primary
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    visible: titleHover.containsMouse && !titleField.activeFocus
                    opacity: 0.8
                }
            }

            RowLayout {
                spacing: 8


                Rectangle { width: 4; height: 4; radius: 2; color: Theme.outline }

                Text {
                    text: root.startTimeText
                    font.family: Fonts.bodyFamily
                    font.pixelSize: Fonts.small
                    font.bold: true
                    color: Theme.textOnSurfaceVariant
                }
            }
        }

        // Push content to left
        Item { Layout.fillWidth: true }
    }
}
