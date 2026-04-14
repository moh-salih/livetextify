import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify

ScrollView {
    id: root

    // --- Pure UI Properties ---
    property var questionModel: null

    // --- Pure UI Signals ---
    signal refreshClicked()

    Layout.fillWidth: true
    Layout.fillHeight: true
    clip: true
    contentWidth: availableWidth

    ColumnLayout {
        width: root.availableWidth
        spacing: 16

        RowLayout {
            Layout.fillWidth: true
            Layout.bottomMargin: 8

            ColumnLayout {
                spacing: 4

                Text {
                    text: "Neural Insights"
                    font.family: Fonts.headlineFamily
                    font.pixelSize: 24
                    font.bold: true
                    color: Theme.primary
                }

                Text {
                    text: "AI-generated questions based on the live discussion"
                    font.family: Fonts.bodyFamily
                    font.pixelSize: 14
                    font.weight: Font.Medium
                    color: Theme.textOnSurfaceVariant
                }
            }

            Item { Layout.fillWidth: true }

            Rectangle {
                color: Theme.surfaceContainerHigh
                radius: 12
                border.color: refreshMouseArea.containsMouse ? Theme.primary : Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.2)
                Layout.preferredWidth: 150
                Layout.preferredHeight: 36

                Behavior on border.color { ColorAnimation { duration: 150 } }

                RowLayout {
                    anchors.centerIn: parent
                    spacing: 8

                    Text {
                        id: refreshIcon
                        text: "refresh"
                        font.family: Fonts.iconFamily
                        font.pixelSize: 14
                        color: Theme.primary

                        // Visual spin effect when clicked
                        RotationAnimation {
                            id: spinAnim
                            target: refreshIcon
                            from: 0
                            to: 360
                            duration: 1000
                            easing.type: Easing.InOutQuad
                        }
                    }

                    Text {
                        text: "GENERATE"
                        font.family: Fonts.bodyFamily
                        font.pixelSize: 10
                        font.bold: true
                        font.letterSpacing: 1
                        color: Theme.textOnSurface
                    }
                }

                MouseArea {
                    id: refreshMouseArea
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    onClicked: {
                        spinAnim.restart()
                        root.refreshClicked()
                    }
                }
            }
        }

        // Dynamic generation from the C++ QStringListModel
        Repeater {
            model: root.questionModel

            QuestionItem {
                // QStringListModel exposes its strings via the 'display' role by default in QML
                questionText: model.display

                // Visual defaults for dynamically generated items
                icon: "psychology"
                iconColor: Theme.primary
                category: "AI Insight"
                timeText: "Just now"
            }
        }

        // Empty state placeholder
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            visible: root.questionModel && root.questionModel.count === 0

            Text {
                anchors.centerIn: parent
                text: "No insights generated yet. Click Generate to analyze the transcription."
                font.family: Fonts.bodyFamily
                font.pixelSize: 14
                color: Theme.textOnSurfaceVariant
                opacity: 0.6
            }
        }

        Item {
            Layout.preferredHeight: 16
            Layout.fillWidth: true
        }
    }

    component QuestionItem: Rectangle {
        id: qItem

        property string icon
        property color iconColor
        property string category
        property string timeText
        property string questionText

        Layout.fillWidth: true
        implicitHeight: Math.max(140, mainRow.implicitHeight + 48)

        color: Qt.rgba(Theme.surfaceContainerLow.r, Theme.surfaceContainerLow.g, Theme.surfaceContainerLow.b, 0.4)
        radius: 24
        border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.1)

        RowLayout {
            id: mainRow
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 24
            spacing: 24

            Rectangle {
                width: 48
                height: 48
                radius: 16
                color: Qt.rgba(qItem.iconColor.r, qItem.iconColor.g, qItem.iconColor.b, 0.1)
                Layout.alignment: Qt.AlignTop

                Text {
                    anchors.centerIn: parent
                    text: qItem.icon
                    font.family: Fonts.iconFamily
                    font.pixelSize: 24
                    color: qItem.iconColor
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 8

                RowLayout {
                    spacing: 12
                    Rectangle {
                        color: Qt.rgba(qItem.iconColor.r, qItem.iconColor.g, qItem.iconColor.b, 0.05)
                        radius: 4
                        Layout.preferredHeight: 20
                        Layout.preferredWidth: catText.implicitWidth + 16

                        Text {
                            id: catText
                            anchors.centerIn: parent
                            text: qItem.category
                            font.family: Fonts.bodyFamily
                            font.pixelSize: 10
                            font.bold: true
                            font.letterSpacing: 2
                            font.capitalization: Font.AllUppercase
                            color: qItem.iconColor
                        }
                    }
                    Text {
                        text: qItem.timeText
                        font.family: Fonts.bodyFamily
                        font.pixelSize: 10
                        font.weight: Font.Medium
                        color: Theme.textOnSurfaceVariant
                    }
                }

                Text {
                    text: qItem.questionText
                    font.family: Fonts.headlineFamily
                    font.pixelSize: 18
                    font.weight: Font.Medium
                    color: Theme.textOnSurface
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                    lineHeight: 1.2
                }

                RowLayout {
                    Layout.topMargin: 4
                    spacing: 16

                    Item {
                        Layout.preferredWidth: askRow.implicitWidth
                        Layout.preferredHeight: askRow.implicitHeight

                        RowLayout {
                            id: askRow
                            anchors.centerIn: parent
                            spacing: 6

                            Text {
                                text: "send"
                                font.family: Fonts.iconFamily
                                font.pixelSize: 16
                                color: maAsk.containsMouse ? Theme.primary : Theme.textOnSurfaceVariant
                            }

                            Text {
                                text: "Ask Panel"
                                font.family: Fonts.bodyFamily
                                font.pixelSize: 11
                                font.bold: true
                                color: maAsk.containsMouse ? Theme.primary : Theme.textOnSurfaceVariant
                            }
                        }

                        MouseArea {
                            id: maAsk
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            hoverEnabled: true
                        }
                    }
                }
            }
        }
    }
}
