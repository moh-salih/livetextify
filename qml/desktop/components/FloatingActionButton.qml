import QtQuick
import QtQuick.Layouts
import QtQuick.Effects
import "../singletons"

Rectangle {
    id: root
    property string icon: ""
    property string text: ""

    signal clicked()

    width: fabRow.implicitWidth + 48
    height: 56
    radius: 28
    gradient: Theme.primaryGradient

    layer.enabled: true
    layer.effect: MultiEffect {
        shadowEnabled: true
        shadowColor: Theme.primary
        shadowOpacity: 0.5
        shadowBlur: 24
        shadowVerticalOffset: 6
    }

    RowLayout {
        id: fabRow
        anchors.centerIn: parent
        spacing: 12

        Text {
            text: root.icon
            font.family: Fonts.iconFamily
            font.pixelSize: 24
            color: Theme.background
        }
        Text {
            text: root.text
            font.family: Fonts.headlineFamily
            font.pixelSize: 14
            font.bold: true
            color: Theme.background
        }
    }

    MouseArea {
        id: fabMouseArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true
        onPressed: root.scale = 0.95
        onReleased: root.scale = 1.0
        onClicked: root.clicked()
    }

    Rectangle {
        anchors.fill: parent
        radius: parent.radius
        color: "#ffffff"
        opacity: fabMouseArea.containsMouse ? 0.2 : 0.0
        Behavior on opacity { NumberAnimation { duration: 150 } }
    }

    Behavior on scale { NumberAnimation { duration: 100 } }
}
