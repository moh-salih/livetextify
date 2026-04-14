import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify

RowLayout {
    id: root
    property string title
    property string description
    default property alias control: controlLayout.data

    spacing: 24

    ColumnLayout {
        Layout.fillWidth: true
        spacing: 4

        Text {
            text: root.title
            font.family: Fonts.headlineFamily
            font.pixelSize: Fonts.title
            font.bold: true
            color: Theme.textOnSurface
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        Text {
            text: root.description
            font.family: Fonts.bodyFamily
            font.pixelSize: Fonts.normal
            color: Theme.textOnSurfaceVariant
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
    }

    // FIX: Using a RowLayout guarantees injected controls are sized securely
    RowLayout {
        id: controlLayout
        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
        spacing: 8
    }
}
