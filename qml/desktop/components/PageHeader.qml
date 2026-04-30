import QtQuick
import QtQuick.Layouts
import "../singletons"

ColumnLayout {
    id: root
    property string title: ""
    property string description: ""
    property color titleColor: Theme.textOnSurface
    property int titleSize: Fonts.displaySmall

    spacing: 8

    Text {
        text: root.title
        font.family: Fonts.headlineFamily
        font.pixelSize: root.titleSize
        font.bold: true
        color: root.titleColor
        Layout.fillWidth: true
    }

    Text {
        visible: root.description !== ""
        text: root.description
        font.family: Fonts.bodyFamily
        font.pixelSize: Fonts.title
        color: Theme.textOnSurfaceVariant
        Layout.maximumWidth: 800
        wrapMode: Text.WordWrap
        Layout.fillWidth: true
    }
}
