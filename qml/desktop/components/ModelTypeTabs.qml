import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify

Rectangle {
    id: root
    property int currentTab: 0
    signal tabClicked(int index)

    height: 56
    width: mainRow.implicitWidth + 12
    radius: 28
    color: Theme.surfaceContainerLow

    RowLayout {
        id: mainRow
        anchors.fill: parent
        anchors.margins: 6
        spacing: 8

        TabPill { tabIndex: 0; icon: "neurology"; label: "LLaMA" }
        TabPill { tabIndex: 1; icon: "mic"; label: "Whisper" }
        TabPill { tabIndex: 2; icon: "hub"; label: "Embedding" }
    }

    component TabPill: Rectangle {
        id: pill
        property int tabIndex
        property string icon
        property string label
        property bool isActive: root.currentTab === tabIndex

        Layout.preferredHeight: 44
        Layout.preferredWidth: contentRow.implicitWidth + 32
        radius: 22

        // FIX: Purely declarative handling of hover and active states
        color: isActive ? Theme.primary : (hoverArea.containsMouse ? Theme.surfaceContainerHigh : "transparent")

        RowLayout {
            id: contentRow
            anchors.centerIn: parent
            spacing: 8

            Text {
                text: pill.icon
                font.family: Fonts.iconFamily
                font.pixelSize: Fonts.title
                color: pill.isActive ? Theme.background : Theme.textOnSurfaceVariant
            }

            Text {
                text: pill.label
                font.family: Fonts.bodyFamily
                font.pixelSize: Fonts.normal
                font.bold: true
                color: pill.isActive ? Theme.background : Theme.textOnSurfaceVariant
            }
        }

        MouseArea {
            id: hoverArea
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            hoverEnabled: true
            onClicked: {
                root.currentTab = tabIndex
                root.tabClicked(tabIndex)
            }
        }

        Behavior on color { ColorAnimation { duration: 150 } }
    }
}
