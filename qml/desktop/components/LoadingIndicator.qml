import QtQuick
import QtQuick.Layouts
import LiveTextify
import "../singletons"

RowLayout {
    property string text: "Loading..."

    Layout.alignment: Qt.AlignHCenter
    Layout.topMargin: 40
    Layout.bottomMargin: 40

    BouncingDots {}

    Text {
        text: parent.text
        font.family: Fonts.bodyFamily
        font.pixelSize: Fonts.small
        font.bold: true
        font.letterSpacing: 1
        color: Theme.primary
        Layout.leftMargin: 12
    }
}
