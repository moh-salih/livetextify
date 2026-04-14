import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify

ColumnLayout {
    id: root
    property string label: ""
    property string hint: ""
    property alias text: inputField.text
    property string placeholderText: ""
    property bool isDropdown: false

    spacing: 8

    Text {
        text: root.label
        font.family: Fonts.headlineFamily
        font.pixelSize: 12
        font.bold: true
        color: Theme.textOnSurface
    }

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 40
        radius: 8
        color: Theme.surfaceContainerLowest
        border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.3)

        TextField {
            id: inputField
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: root.isDropdown ? 40 : 16
            color: Theme.textOnSurface
            font.family: Fonts.bodyFamily
            font.pixelSize: 14
            background: Item{}
            verticalAlignment: TextInput.AlignVCenter
            placeholderText: root.placeholderText
            placeholderTextColor: Theme.textOnSurfaceVariant
        }

        Text {
            visible: root.isDropdown
            anchors.right: parent.right
            anchors.rightMargin: 12
            anchors.verticalCenter: parent.verticalCenter
            text: "expand_more"
            font.family: Fonts.iconFamily
            font.pixelSize: 20
            color: Theme.textOnSurfaceVariant
        }
    }

    Text {
        visible: root.hint !== ""
        text: root.hint
        font.family: Fonts.bodyFamily
        font.pixelSize: 10
        color: Theme.textOnSurfaceVariant
    }
}
