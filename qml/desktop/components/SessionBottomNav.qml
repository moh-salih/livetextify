import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify
import "../singletons"

Rectangle {
    id: root
    property int currentTabIndex: 0
    property int orientation: Qt.Horizontal

    // --- Display Mode Controls ---
    property bool showIcon: true
    // Automatically hide text in Clear Mode
    property bool showText: !Theme.isClearMode

    signal tabClicked(int index)

    // Dynamic width with smooth animation
    implicitWidth: orientation === Qt.Horizontal ? 420 : (showText ? 110 : 72)
    implicitHeight: orientation === Qt.Horizontal ? 64 : 300

    Behavior on implicitWidth {
        NumberAnimation { duration: 250; easing.type: Easing.OutQuart }
    }

    // Adapt background for transparent vertical rail
    color: orientation === Qt.Horizontal ? Qt.rgba(Theme.surfaceContainerHigh.r, Theme.surfaceContainerHigh.g, Theme.surfaceContainerHigh.b, 0.9) : Qt.rgba(Theme.surfaceContainerLow.r, Theme.surfaceContainerLow.g, Theme.surfaceContainerLow.b, 0.4)
    radius: orientation === Qt.Horizontal ? 32 : 0
    border.color: orientation === Qt.Horizontal ? Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.2) : "transparent"

    // Right border line for vertical mode
    Rectangle {
        visible: orientation === Qt.Vertical
        width: 1; height: parent.height
        anchors.right: parent.right
        color: Qt.rgba(Theme.outlineVariant.r, Theme.outlineVariant.g, Theme.outlineVariant.b, 0.1)
    }

    Loader {
        anchors.fill: parent
        anchors.margins: orientation === Qt.Horizontal ? 6 : 8
        anchors.topMargin: orientation === Qt.Vertical ? 32 : 6
        sourceComponent: orientation === Qt.Horizontal ? horizontalLayout : verticalLayout
    }

    Component {
        id: horizontalLayout
        RowLayout {
            anchors.fill: parent
            spacing: 0

            NavTab { tabIndex: 0; icon: "description"; label: "TRANSCRIPT"; Layout.fillWidth: true; Layout.fillHeight: true }
            NavTab { tabIndex: 1; icon: "forum"; label: "CHAT"; Layout.fillWidth: true; Layout.fillHeight: true }
            // NavTab { tabIndex: 2; icon: "quiz"; label: "Q&A"; Layout.fillWidth: true; Layout.fillHeight: true }
        }
    }

    Component {
        id: verticalLayout
        ColumnLayout {
            anchors.fill: parent
            spacing: 16
            Layout.alignment: Qt.AlignTop

            NavTab { tabIndex: 0; icon: "description"; label: "TRANSCRIPT"; Layout.fillWidth: true }
            NavTab { tabIndex: 1; icon: "forum"; label: "CHAT"; Layout.fillWidth: true }
            NavTab { tabIndex: 2; icon: "quiz"; label: "Q&A"; Layout.fillWidth: true }
            Item { Layout.fillHeight: true } // push to top
        }
    }

    component NavTab: Rectangle {
        id: navTabRoot
        property int tabIndex
        property string icon
        property string label
        property bool isActive: root.currentTabIndex === navTabRoot.tabIndex

        color: root.orientation === Qt.Horizontal ? "transparent" : (isActive ? Theme.primary : "transparent")
        gradient: root.orientation === Qt.Horizontal && isActive ? Theme.primaryGradient : null

        radius: root.orientation === Qt.Horizontal ? 26 : 16

        // Dynamic sizing mapping based on Display Mode
        Layout.preferredHeight: root.orientation === Qt.Vertical ? (root.showText ? 64 : 56) : -1
        Layout.preferredWidth: root.orientation === Qt.Vertical ? (root.showText ? 94 : 56) : -1
        Layout.alignment: Qt.AlignHCenter

        Behavior on Layout.preferredWidth { NumberAnimation { duration: 250; easing.type: Easing.OutQuart } }
        Behavior on Layout.preferredHeight { NumberAnimation { duration: 250; easing.type: Easing.OutQuart } }

        ColumnLayout {
            anchors.centerIn: parent
            spacing: root.orientation === Qt.Horizontal ? 2 : 4

            Text {
                visible: root.showIcon
                text: navTabRoot.icon
                font.family: Fonts.iconFamily
                font.pixelSize: root.orientation === Qt.Horizontal ? 18 : 24
                Layout.alignment: Qt.AlignHCenter
                color: navTabRoot.isActive ? Theme.background : (hoverArea.containsMouse ? Theme.textOnSurface : Theme.textOnSurfaceVariant)
            }

            Text {
                visible: root.showText
                text: navTabRoot.label
                font.family: Fonts.headlineFamily
                font.pixelSize: root.orientation === Qt.Horizontal ? 8 : 9
                font.bold: true
                font.letterSpacing: 1
                Layout.alignment: Qt.AlignHCenter
                color: navTabRoot.isActive ? Theme.background : (hoverArea.containsMouse ? Theme.textOnSurface : Theme.textOnSurfaceVariant)
            }
        }

        MouseArea {
            id: hoverArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: root.tabClicked(navTabRoot.tabIndex)
        }
        Behavior on color { ColorAnimation { duration: 150 } }
    }
}
