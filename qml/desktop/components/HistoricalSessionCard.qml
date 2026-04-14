import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify

Rectangle {
    id: root
    property string title
    property string dateText
    property string durationText
    property bool isRagIndexed: false

    signal detailsClicked()

    Layout.preferredHeight: 160
    radius: 32
    color: Qt.rgba(Theme.surfaceContainerHigh.r, Theme.surfaceContainerHigh.g, Theme.surfaceContainerHigh.b, 0.4)
    border.color: hoverArea.containsMouse ? Qt.rgba(Theme.primary.r, Theme.primary.g, Theme.primary.b, 0.3) : Qt.rgba(255, 255, 255, 0.08)

    // Hover translate effect
    transform: Translate {
        y: hoverArea.containsMouse ? -4 : 0
        Behavior on y { NumberAnimation { duration: 300; easing.type: Easing.OutQuart } }
    }

    Behavior on border.color { ColorAnimation { duration: 300 } }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 28

        // Top Row: Info + Badge
        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 8

                Text {
                    text: root.title
                    font.family: Fonts.headlineFamily
                    font.pixelSize: Fonts.largeTitle
                    font.bold: true
                    color: hoverArea.containsMouse ? Theme.primary : Theme.textOnSurface

                    // FIX: Force the text to shrink below its implicit width
                    Layout.fillWidth: true
                    Layout.preferredWidth: 0
                    elide: Text.ElideRight
                    Behavior on color { ColorAnimation { duration: 300 } }
                }

                RowLayout {
                    spacing: 16

                    RowLayout {
                        spacing: 6
                        Text { text: "calendar_today"; font.family: Fonts.iconFamily; font.pixelSize: Fonts.title; color: Theme.primary; opacity: 0.8 }
                        Text { text: root.dateText; font.family: Fonts.bodyFamily; font.pixelSize: Fonts.normal; font.bold: true; color: Theme.textOnSurfaceVariant }
                    }

                    RowLayout {
                        spacing: 6
                        Text { text: "schedule"; font.family: Fonts.iconFamily; font.pixelSize: Fonts.title; color: Theme.textOnSurface; opacity: 0.6 }
                        Text { text: root.durationText; font.family: Fonts.bodyFamily; font.pixelSize: Fonts.normal; font.bold: true; color: Theme.textOnSurfaceVariant; opacity: 0.8 }
                    }
                }
            }

            // RAG Status Badge
            Rectangle {
                Layout.alignment: Qt.AlignTop
                Layout.preferredHeight: 28
                Layout.preferredWidth: badgeRow.implicitWidth + 24
                radius: 14
                color: root.isRagIndexed ? Qt.rgba(Theme.secondary.r, Theme.secondary.g, Theme.secondary.b, 0.15) : Qt.rgba(255, 255, 255, 0.05)
                border.color: root.isRagIndexed ? Qt.rgba(Theme.secondary.r, Theme.secondary.g, Theme.secondary.b, 0.3) : Qt.rgba(255, 255, 255, 0.1)

                RowLayout {
                    id: badgeRow
                    anchors.centerIn: parent
                    spacing: 8

                    Rectangle {
                        width: 8; height: 8; radius: 4
                        color: root.isRagIndexed ? Theme.secondary : Theme.outline
                        SequentialAnimation on opacity {
                            running: root.isRagIndexed
                            loops: Animation.Infinite
                            NumberAnimation { to: 0.4; duration: 1000 }
                            NumberAnimation { to: 1.0; duration: 1000 }
                        }
                    }

                    Text {
                        text: root.isRagIndexed ? "RAG INDEXED" : "PROCESSING"
                        font.family: Fonts.bodyFamily
                        font.pixelSize: Fonts.tiny
                        font.bold: true
                        font.letterSpacing: 1.5
                        color: root.isRagIndexed ? Theme.secondary : Theme.outline
                    }
                }
            }
        }

        Item { Layout.fillHeight: true }

        // Bottom Row: Details Button
        RowLayout {
            Layout.fillWidth: true

            Item { Layout.fillWidth: true } // pushes button right

            Rectangle {
                Layout.preferredWidth: detailsRow.implicitWidth + 32
                Layout.preferredHeight: 36
                radius: 8
                color: detailsHover.containsMouse ? Theme.primary : Qt.rgba(255, 255, 255, 0.05)
                border.color: Qt.rgba(255, 255, 255, 0.05)

                RowLayout {
                    id: detailsRow
                    anchors.centerIn: parent
                    spacing: 8

                    Text {
                        text: "Details"
                        font.family: Fonts.bodyFamily
                        font.pixelSize: Fonts.small
                        font.bold: true
                        color: detailsHover.containsMouse ? Theme.background : Theme.primary
                    }
                    Text {
                        text: "arrow_forward"
                        font.family: Fonts.iconFamily
                        font.pixelSize: Fonts.normal
                        color: detailsHover.containsMouse ? Theme.background : Theme.primary
                    }
                }

                MouseArea {
                    id: detailsHover
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    onClicked: root.detailsClicked()
                }

                Behavior on color { ColorAnimation { duration: 150 } }
            }
        }
    }

    MouseArea {
        id: hoverArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: root.detailsClicked()
    }
}
