import QtQuick
import QtQuick.Controls
import "../singletons"

Item {
    id: root

    signal clicked();

    // Public API
    property int size: 48
    property string iconText: ""
    property int iconSize: 16
    property color iconColor: Theme.iconDefault
    property string iconFont: Fonts.sharpIcon
    property color backgroundColor: "transparent"


    Button{
        implicitWidth: size
        implicitHeight: implicitWidth
        onClicked: root.clicked();

        background: Rectangle {
            anchors.fill: parent
            radius: width / 2
            color: root.backgroundColor
            opacity: parent.down ? 0.8 : 1
        }

        contentItem: Label {
            anchors.centerIn: parent
            width: parent.width
            height: parent.height

            text: root.iconText
            font.family: root.iconFont
            font.pixelSize: root.iconSize
            color: root.iconColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
}
