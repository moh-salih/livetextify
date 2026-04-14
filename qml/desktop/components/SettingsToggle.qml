import QtQuick
import LiveTextify

Rectangle {
    id: root
    property bool checked: false
    property color themeColor: Theme.primary

    width: 44; height: 24; radius: 12
    color: checked ? themeColor : Theme.surfaceContainerHighest

    Rectangle {
        width: 16; height: 16; radius: 8
        color: checked ? Theme.background : Theme.textOnSurfaceVariant
        anchors.verticalCenter: parent.verticalCenter
        x: checked ? 24 : 4
        Behavior on x { NumberAnimation { duration: 150; easing.type: Easing.OutQuad } }
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: root.checked = !root.checked
    }
}
