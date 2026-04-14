import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify

ColumnLayout {
    id: root
    property string label: ""
    property string valueText: ""
    property real from: 0
    property real to: 100
    property real value: 50
    property real stepSize: 1
    property color themeColor: Theme.primary
    property string hint: ""

    spacing: 12

    RowLayout {
        Layout.fillWidth: true
        Text {
            text: root.label
            font.family: Fonts.headlineFamily
            font.pixelSize: 14
            font.bold: true
            color: Theme.textOnSurface
        }
        Item { Layout.fillWidth: true }
        Rectangle {
            visible: root.valueText !== ""
            color: Theme.surfaceContainerLowest
            radius: 6
            border.color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.3)
            Layout.preferredHeight: 24
            Layout.preferredWidth: valText.implicitWidth + 16

            Text {
                id: valText
                anchors.centerIn: parent
                text: root.valueText
                font.family: Fonts.headlineFamily
                font.pixelSize: 12
                font.bold: true
                color: root.themeColor
            }
        }
    }

    Slider {
        id: control
        Layout.fillWidth: true
        from: root.from
        to: root.to
        value: root.value
        stepSize: root.stepSize

        background: Rectangle {
            x: control.leftPadding
            y: control.topPadding + control.availableHeight / 2 - height / 2
            implicitWidth: 200
            implicitHeight: 6
            width: control.availableWidth
            height: implicitHeight
            radius: 3
            color: Theme.surfaceContainerLowest

            Rectangle {
                width: control.visualPosition * parent.width
                height: parent.height
                color: root.themeColor
                radius: 3
            }
        }
        handle: Rectangle {
            x: control.leftPadding + control.visualPosition * (control.availableWidth - width)
            y: control.topPadding + control.availableHeight / 2 - height / 2
            implicitWidth: 16
            implicitHeight: 16
            radius: 8
            color: root.themeColor
            border.color: Theme.background
            border.width: 2
        }

        onValueChanged: root.value = value
    }

    Text {
        visible: root.hint !== ""
        text: root.hint
        font.family: Fonts.bodyFamily
        font.pixelSize: 10
        color: Theme.textOnSurfaceVariant
        font.capitalization: Font.AllUppercase
        font.letterSpacing: 1
    }
}
