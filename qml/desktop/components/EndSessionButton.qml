import QtQuick
import QtQuick.Layouts
import LiveTextify
import "../singletons"

Rectangle {
    id: root
    signal clicked()

    width: contentRow.implicitWidth + 48
    height: 52
    radius: 16
    color: endSessionMouseArea.containsMouse ? "#D32F2F" : "#E53935"

    // Glow/Shadow effect
    Rectangle {
        anchors.centerIn: parent
        width: parent.width + 12
        height: parent.height + 12
        radius: 20
        color: Qt.rgba(229/255, 57/255, 53/255, 0.3) // #E53935 with 30% opacity
        z: -1
    }

    RowLayout {
        id: contentRow
        anchors.centerIn: parent
        spacing: 12

        Text {
            text: "power_settings_new"
            font.family: Fonts.iconFamily
            font.pixelSize: 20
            font.bold: true
            color: "white"
        }
    }

    MouseArea {
        id: endSessionMouseArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true
        onPressed: root.scale = 0.95
        onReleased: root.scale = 1.0
        onClicked: root.clicked()
    }

    Behavior on scale { NumberAnimation { duration: 100 } }
    Behavior on color { ColorAnimation { duration: 150 } }
}
