import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify
import "../singletons"

Rectangle {
    id: root

    // --- Pure UI Properties ---
    property var chatModel: null
    property bool isGenerating: false

    // --- Pure UI Signals ---
    signal sendMessage(string text)
    signal stopGeneration()

    Layout.fillWidth: true
    Layout.fillHeight: true

    color: Qt.rgba(Theme.surfaceContainerLow.r, Theme.surfaceContainerLow.g, Theme.surfaceContainerLow.b, 0.3)
    radius: 24
    border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.2)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 24

        ListView {
            id: chatList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 24

            model: root.chatModel

            // onCountChanged: Qt.callLater(() => chatList.positionViewAtEnd())

            delegate: ChatMessage {
                isUser: model.role === "user"
                textContent: model.content
                isStreaming: !model.isComplete && !isUser
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 56
            color: Qt.rgba(Theme.surfaceContainerHighest.r, Theme.surfaceContainerHighest.g, Theme.surfaceContainerHighest.b, 0.6)
            radius: 16
            border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.3)

            TextField {
                id: chatInput
                anchors.fill: parent
                anchors.leftMargin: 24
                anchors.rightMargin: 64
                placeholderText: "Type a message..."
                font.family: Fonts.bodyFamily
                font.pixelSize: 14
                font.weight: Font.Medium
                color: Theme.textOnSurface
                background: Item {}
                verticalAlignment: TextInput.AlignVCenter

                onAccepted: {
                    if (text.trim() === "") return

                    if (root.isGenerating) {
                        root.stopGeneration()
                    }
                    root.sendMessage(text.trim())
                    text = ""
                }
            }

            Rectangle {
                width: 40
                height: 40
                radius: 12
                color: Theme.primary
                anchors.right: parent.right
                anchors.rightMargin: 8
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    text: root.isGenerating ? "stop" : "send"
                    font.family: Fonts.iconFamily
                    font.pixelSize: 18
                    color: Theme.textOnPrimary
                    anchors.centerIn: parent
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onPressed: parent.scale = 0.95
                    onReleased: parent.scale = 1.0
                    onClicked: {
                        if (root.isGenerating) {
                            root.stopGeneration()
                        } else {
                            if (chatInput.text.trim() === "") return
                            root.sendMessage(chatInput.text.trim())
                            chatInput.text = ""
                        }
                    }
                }

                Behavior on scale { NumberAnimation { duration: 100 } }
            }
        }
    }

    component ChatMessage: Item {
        id: msgRoot
        property bool isUser: false
        property string textContent
        property bool isStreaming: false

        width: ListView.view.width
        implicitHeight: mainLayout.implicitHeight

        ColumnLayout {
            id: mainLayout
            anchors.left: msgRoot.isUser ? undefined : parent.left
            anchors.right: msgRoot.isUser ? parent.right : undefined
            spacing: 8

            Text {
                Layout.alignment: msgRoot.isUser ? Qt.AlignRight : Qt.AlignLeft
                Layout.leftMargin: msgRoot.isUser ? 0 : 16
                Layout.rightMargin: msgRoot.isUser ? 16 : 0
                text: msgRoot.isUser ? "" : "ASSISTANT"
                font.family: Fonts.bodyFamily
                font.pixelSize: 10
                font.bold: true
                font.letterSpacing: 1
                color: msgRoot.isUser ? Theme.textOnSurfaceVariant : Qt.rgba(Theme.primary.r, Theme.primary.g, Theme.primary.b, 0.6)
            }

            Rectangle {
                id: bubble
                Layout.alignment: msgRoot.isUser ? Qt.AlignRight : Qt.AlignLeft
                Layout.maximumWidth: msgRoot.width * 0.85

                implicitWidth: Math.max(40, contentCol.implicitWidth + 40)
                implicitHeight: contentCol.implicitHeight + 32

                radius: 20
                color: msgRoot.isUser ? "transparent" : Qt.rgba(Theme.surfaceContainerHighest.r, Theme.surfaceContainerHighest.g, Theme.surfaceContainerHighest.b, 0.8)
                border.color: msgRoot.isUser ? "transparent" : Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.1)

                Rectangle {
                    anchors.fill: parent
                    radius: parent.radius
                    gradient: Theme.primaryGradient
                    visible: msgRoot.isUser
                    z: -2
                }

                ColumnLayout {
                    id: contentCol
                    anchors.centerIn: parent
                    width: parent.width - 40
                    spacing: 8

                    Text {
                        id: msgText
                        Layout.fillWidth: true
                        text: msgRoot.textContent
                        font.family: Fonts.bodyFamily
                        font.pixelSize: 14
                        font.weight: msgRoot.isUser ? Font.Bold : Font.Medium
                        color: msgRoot.isUser ? Theme.background : Theme.textOnSurface
                        lineHeight: 1.5
                        wrapMode: Text.WordWrap
                    }

                    BouncingDots {
                        visible: msgRoot.isStreaming
                    }
                }
            }
        }
    }
}
