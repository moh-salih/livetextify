import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify

Rectangle {
    id: root

    property string modelName: ""
    property string fileSize: ""
    property string status: "available"
    property real downloadProgress: 0.0
    property string downloadSpeed: ""
    property string downloadAmount: ""
    property string iconName: status === "downloading" ? "psychology" : (status === "available" ? "cloud_download" : "terminal")
    property color cardIconColor: status === "downloading" ? Theme.secondary : (status === "available" ? Theme.textOnSurfaceVariant : Theme.primary)

    signal downloadClicked()
    signal cancelClicked()
    signal activateClicked()
    signal deleteClicked()

    Layout.fillWidth: true
    Layout.preferredHeight: status === "downloading" ? 110 : 96
    radius: 24
    color: Theme.surfaceContainerLow

    Behavior on color { ColorAnimation { duration: 150 } }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: status === "available" ? Qt.PointingHandCursor : Qt.ArrowCursor
        onEntered: parent.color = Theme.surfaceContainerHigh
        onExited: parent.color = Theme.surfaceContainerLow
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 20

        Rectangle {
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: 56
            Layout.preferredHeight: 56
            radius: 16
            color: Theme.surfaceContainerLowest

            Text {
                anchors.centerIn: parent
                text: root.iconName
                font.family: Fonts.iconFamily
                font.pixelSize: 32
                color: root.cardIconColor
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            spacing: 4

            RowLayout {
                spacing: 8
                Text {
                    text: root.modelName
                    font.family: Fonts.headlineFamily
                    font.pixelSize: Fonts.heading
                    font.bold: true
                    color: Theme.textOnSurface

                    // FIX: Allow excessively long model names to be dotted out
                    Layout.fillWidth: true
                    Layout.preferredWidth: 0
                    elide: Text.ElideRight
                }

                Rectangle {
                    visible: root.status === "default"
                    color: Qt.rgba(Theme.primary.r, Theme.primary.g, Theme.primary.b, 0.2)
                    radius: 4
                    Layout.preferredHeight: 20
                    Layout.preferredWidth: defaultText.implicitWidth + 16

                    Text {
                        id: defaultText
                        anchors.centerIn: parent
                        text: "DEFAULT"
                        font.family: Fonts.bodyFamily
                        font.pixelSize: Fonts.tiny
                        font.bold: true
                        color: Theme.primary
                        font.letterSpacing: 1
                    }
                }
            }

            RowLayout {
                visible: root.status !== "downloading"
                spacing: 16

                RowLayout {
                    spacing: 4
                    Text { text: "folder"; font.family: Fonts.iconFamily; font.pixelSize: Fonts.small; color: Theme.textOnSurfaceVariant }
                    Text { text: root.fileSize; font.family: Fonts.bodyFamily; font.pixelSize: Fonts.normal; color: Theme.textOnSurfaceVariant }
                }

                RowLayout {
                    visible: root.status === "downloaded" || root.status === "default"
                    spacing: 4
                    Text { text: "check_circle"; font.family: Fonts.iconFamily; font.pixelSize: Fonts.small; color: "#4ade80" }
                    Text { text: "Downloaded"; font.family: Fonts.bodyFamily; font.pixelSize: Fonts.normal; color: Theme.textOnSurfaceVariant }
                }

                Text {
                    visible: root.status === "available"
                    text: "Available on Cloud"
                    font.family: Fonts.bodyFamily
                    font.pixelSize: Fonts.normal
                    font.italic: true
                    color: Theme.textOnSurfaceVariant
                    opacity: 0.5
                }
            }

            ColumnLayout {
                visible: root.status === "downloading"
                Layout.fillWidth: true
                spacing: 8
                Layout.topMargin: 4

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 6
                    radius: 3
                    color: Theme.surfaceContainerLowest

                    Rectangle {
                        width: parent.width * root.downloadProgress
                        height: parent.height
                        radius: 3
                        color: Theme.secondary
                        Behavior on width { NumberAnimation { duration: 300 } }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        text: "DOWNLOADING: " + Math.round(root.downloadProgress * 100) + "%"
                        font.family: Fonts.bodyFamily
                        font.pixelSize: 11
                        font.bold: true
                        color: Theme.textOnSurfaceVariant
                        font.letterSpacing: 1
                    }
                }
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            spacing: 12

            // Activate Button
            Rectangle {
                visible: root.status === "downloaded"
                Layout.preferredWidth: 100
                Layout.preferredHeight: 36
                radius: 8
                color: Theme.surfaceContainerHighest
                border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.4)

                Text {
                    anchors.centerIn: parent
                    text: "Activate"
                    font.family: Fonts.bodyFamily
                    font.pixelSize: Fonts.normal
                    font.bold: true
                    color: Theme.textOnSurface
                }
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    onEntered: parent.border.color = Qt.rgba(Theme.primary.r, Theme.primary.g, Theme.primary.b, 0.4)
                    onExited: parent.border.color = Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.4)
                    onClicked: root.activateClicked()
                }
            }

            // Delete Button
            Rectangle {
                visible: root.status === "downloaded" || root.status === "default"
                Layout.preferredWidth: 36
                Layout.preferredHeight: 36
                radius: 8
                color: "transparent"

                Text {
                    anchors.centerIn: parent
                    text: "delete"
                    font.family: Fonts.iconFamily
                    font.pixelSize: 20
                    color: deleteHover.containsMouse ? "#ff716c" : Theme.textOnSurfaceVariant
                    Behavior on color { ColorAnimation { duration: 150 } }
                }
                MouseArea {
                    id: deleteHover
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    onEntered: parent.color = Qt.rgba(255, 113, 108, 0.1)
                    onExited: parent.color = "transparent"
                    onClicked: root.deleteClicked()
                }
            }

            // Downloading State Buttons
            Rectangle {
                visible: root.status === "downloading"
                Layout.preferredWidth: 36
                Layout.preferredHeight: 36
                radius: 8
                color: "transparent"

                Text {
                    anchors.centerIn: parent
                    text: "close"
                    font.family: Fonts.iconFamily
                    font.pixelSize: 20
                    color: Theme.textOnSurfaceVariant
                }
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    onEntered: parent.color = Theme.surfaceContainerLowest
                    onExited: parent.color = "transparent"
                    onClicked: root.cancelClicked()
                }
            }

            // Available State Button
            Rectangle {
                visible: root.status === "available"
                Layout.preferredWidth: 120
                Layout.preferredHeight: 40
                radius: 8
                gradient: Theme.primaryGradient

                RowLayout {
                    anchors.centerIn: parent
                    spacing: 8
                    Text { text: "download"; font.family: Fonts.iconFamily; font.pixelSize: Fonts.normal; color: Theme.background }
                    Text { text: "Download"; font.family: Fonts.bodyFamily; font.pixelSize: Fonts.normal; font.bold: true; color: Theme.background }
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onPressed: parent.scale = 0.95
                    onReleased: parent.scale = 1.0
                    onClicked: root.downloadClicked()
                }
                Behavior on scale { NumberAnimation { duration: 100 } }
            }
        }
    }
}
