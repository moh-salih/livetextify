import QtQuick

Item {
    id: root

    // Data flows downward (Rule 4 & 5)
    property int gripSize: 6

    // Actions flow upward (Rule 3 & 5)
    signal resizeRequested(int edge)

    // --- Edges ---

    // Top Edge
    MouseArea {
        height: root.gripSize
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: root.gripSize
        anchors.rightMargin: root.gripSize
        cursorShape: Qt.SizeVerCursor
        onPressed: root.resizeRequested(Qt.TopEdge)
    }

    // Bottom Edge
    MouseArea {
        height: root.gripSize
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: root.gripSize
        anchors.rightMargin: root.gripSize
        cursorShape: Qt.SizeVerCursor
        onPressed: root.resizeRequested(Qt.BottomEdge)
    }

    // Left Edge
    MouseArea {
        width: root.gripSize
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.topMargin: root.gripSize
        anchors.bottomMargin: root.gripSize
        cursorShape: Qt.SizeHorCursor
        onPressed: root.resizeRequested(Qt.LeftEdge)
    }

    // Right Edge
    MouseArea {
        width: root.gripSize
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.topMargin: root.gripSize
        anchors.bottomMargin: root.gripSize
        cursorShape: Qt.SizeHorCursor
        onPressed: root.resizeRequested(Qt.RightEdge)
    }

    // --- Corners ---

    // Top-Left Corner
    MouseArea {
        width: root.gripSize
        height: root.gripSize
        anchors.top: parent.top
        anchors.left: parent.left
        cursorShape: Qt.SizeFDiagCursor
        onPressed: root.resizeRequested(Qt.TopLeftCorner)
    }

    // Top-Right Corner
    MouseArea {
        width: root.gripSize
        height: root.gripSize
        anchors.top: parent.top
        anchors.right: parent.right
        cursorShape: Qt.SizeBDiagCursor
        onPressed: root.resizeRequested(Qt.TopRightCorner)
    }

    // Bottom-Left Corner
    MouseArea {
        width: root.gripSize
        height: root.gripSize
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        cursorShape: Qt.SizeBDiagCursor
        onPressed: root.resizeRequested(Qt.BottomLeftCorner)
    }

    // Bottom-Right Corner
    MouseArea {
        width: root.gripSize
        height: root.gripSize
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        cursorShape: Qt.SizeFDiagCursor
        onPressed: root.resizeRequested(Qt.BottomRightCorner)
    }
}
