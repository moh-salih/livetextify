import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify

Rectangle {
    id: root
    property alias text: textArea.text
    property string placeholderText: ""

    Layout.fillWidth: true
    Layout.preferredHeight: 100
    radius: 8
    color: Theme.surfaceContainerLowest
    border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.3)

    ScrollView {
        anchors.fill: parent
        anchors.margins: 12
        TextArea {
            id: textArea
            color: Theme.textOnSurface
            font.family: Fonts.bodyFamily
            font.pixelSize: 14
            background: Item {}
            wrapMode: Text.WordWrap
            placeholderText: root.placeholderText
            placeholderTextColor: Theme.textOnSurfaceVariant
        }
    }
}
