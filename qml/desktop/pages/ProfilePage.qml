import QtQuick
import QtQuick.Controls
import LiveTextify
import "../components" as Components
import "../singletons"
Page {
    id: root

    background: Components.PageBackground {
        topColor: Theme.surfaceContainerHigh
        bottomColor: Theme.background
    }


    Text {
        anchors.centerIn: parent
        text: qsTr("Not implemented")
        font.pixelSize: Fonts.display
        color: Theme.outline
    }

    header: Components.PageHeader{
        title: "Profile"
        description: "Show info related to the current user"
    }
}
