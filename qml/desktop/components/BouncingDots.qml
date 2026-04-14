import QtQuick
import QtQuick.Layouts
import LiveTextify

RowLayout {
    spacing: 4

    Repeater {
        model: 3
        Rectangle {
            width: 6
            height: 6
            radius: 3
            color: Theme.primary

            SequentialAnimation on y {
                loops: Animation.Infinite

                PauseAnimation {
                    duration: index * 200
                }
                NumberAnimation {
                    from: 0
                    to: -6
                    duration: 300
                    easing.type: Easing.OutQuad
                }
                NumberAnimation {
                    from: -6
                    to: 0
                    duration: 300
                    easing.type: Easing.InQuad
                }
                PauseAnimation {
                    duration: (2 - index) * 200
                }
            }
        }
    }
}
