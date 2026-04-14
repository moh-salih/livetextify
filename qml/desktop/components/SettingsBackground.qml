import QtQuick
import LiveTextify

Item {
    id: root

    Rectangle {
        anchors.fill: parent
        color: Theme.background
    }

    // Decorative faint glowing orb (Top Right)
    Rectangle {
        width: 500
        height: 500
        radius: 250
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: -150
        anchors.rightMargin: -150
        color: Qt.rgba(Theme.primary.r, Theme.primary.g, Theme.primary.b, 0.03)
    }

    // Decorative faint glowing orb (Bottom Left)
    Rectangle {
        width: 400
        height: 400
        radius: 200
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.bottomMargin: -100
        anchors.leftMargin: -100
        color: Qt.rgba(Theme.secondary.r, Theme.secondary.g, Theme.secondary.b, 0.03)
    }
}
