import QtQuick
import "../singletons"

Rectangle {
    id: root
    property color topColor: Theme.surfaceContainerHighest
    property color bottomColor: Theme.background

    gradient: Gradient {
        GradientStop { position: 0.0; color: root.topColor }
        GradientStop { position: 1.0; color: root.bottomColor }
    }
}
