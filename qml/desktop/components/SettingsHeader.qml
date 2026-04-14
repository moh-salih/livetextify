import QtQuick
import QtQuick.Layouts
import LiveTextify

ColumnLayout {
    id: root
    spacing: 8

    Text {
        text: "Preferences"
        font.family: Fonts.headlineFamily
        font.pixelSize: Fonts.displaySmall
        font.bold: true
        color: Theme.textOnSurface
    }

    Text {
        text: "Customize your environment."
        font.family: Fonts.bodyFamily
        font.pixelSize: Fonts.normal
        color: Theme.textOnSurfaceVariant
        Layout.maximumWidth: 600
        wrapMode: Text.WordWrap
    }
}
