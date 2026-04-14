import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify

Rectangle {
    id: root
    color: Theme.surfaceContainerHighest
    radius: 24

    signal activityClicked()

    Rectangle {
        anchors.fill: parent
        anchors.margins: 4
        color: Theme.surfaceContainerLow
        radius: 20

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 24

            RowLayout {
                Layout.fillWidth: true

                Text {
                    text: "Recent Activity"
                    font.family: Fonts.headlineFamily
                    font.pixelSize: Fonts.heading
                    font.bold: true
                    color: Theme.textOnSurface
                    Layout.fillWidth: true
                }

                Text {
                    text: "2 hours ago"
                    font.family: Fonts.bodyFamily
                    font.pixelSize: Fonts.small
                    color: Theme.textOnSurfaceVariant
                }
            }

            Item {
                Layout.fillHeight: true
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 72
                color: Theme.surfaceContainerLowest
                radius: 12

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 16

                    Rectangle {
                        width: 48
                        height: 48
                        radius: 24
                        color: Qt.rgba(Theme.secondary.r, Theme.secondary.g, Theme.secondary.b, 0.1)

                        Text {
                            anchors.centerIn: parent
                            text: "mic"
                            font.family: Fonts.iconFamily
                            color: Theme.secondary
                            font.pixelSize: Fonts.largeTitle
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 4

                        Text {
                            text: "Strategic Q4 Planning Session"
                            font.family: Fonts.bodyFamily
                            font.pixelSize: Fonts.normal
                            font.bold: true
                            color: Theme.textOnSurface

                            // FIX: Added preferredWidth: 0 to properly engage elision
                            Layout.fillWidth: true
                            Layout.preferredWidth: 0
                            elide: Text.ElideRight
                        }

                        Text {
                            text: "Neural Analysis Completed • 42 mins"
                            font.family: Fonts.bodyFamily
                            font.pixelSize: Fonts.small
                            color: Theme.textOnSurfaceVariant
                        }
                    }

                    Text {
                        text: "chevron_right"
                        font.family: Fonts.iconFamily
                        color: Theme.textOnSurfaceVariant
                        font.pixelSize: Fonts.largeTitle
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    onEntered: parent.color = Qt.rgba(Theme.surfaceContainerLowest.r, Theme.surfaceContainerLowest.g, Theme.surfaceContainerLowest.b, 0.5)
                    onExited: parent.color = Theme.surfaceContainerLowest
                    onClicked: root.activityClicked()
                }
            }
        }
    }
}
