import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import QtQuick.Shapes
import LiveTextify
import "../singletons"

Popup {
    id: root

    // --- Public API ---
    property string titleText: "Synthesizing Intelligence..."
    property string descriptionText: "Retrieving relevant context from your knowledge base..."

    // Convenient method to start the loading indicator
    function start(newTitle = "", newDescription = "") {
        if (newTitle !== "") titleText = newTitle;
        if (newDescription !== "") descriptionText = newDescription;
        root.open();
    }

    // Convenient method to stop the loading indicator
    function stop() {
        root.close();
    }

    // --- Dialog Configuration ---
    width: 450
    padding: 48 // Fixed: Move padding to the Popup itself so the background wraps the content properly
    anchors.centerIn: Overlay.overlay
    modal: true
    dim: true
    closePolicy: Popup.NoAutoClose

    // Smooth Entrance/Exit
    enter: Transition {
        ParallelAnimation {
            NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 300; easing.type: Easing.OutCubic }
            NumberAnimation { property: "scale"; from: 0.95; to: 1.0; duration: 300; easing.type: Easing.OutBack }
        }
    }
    exit: Transition {
        ParallelAnimation {
            NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 200; easing.type: Easing.InCubic }
            NumberAnimation { property: "scale"; from: 1.0; to: 0.95; duration: 200; easing.type: Easing.InCubic }
        }
    }

    // Modal dark backdrop
    Overlay.modal: Rectangle {
        color: Qt.alpha(Theme.background, 0.8)
    }

    // --- Background Panel ---
    background: Item {
        // Deep Drop Shadow
        Rectangle {
            anchors.fill: panel
            color: "black"
            radius: 24
            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true; shadowColor: "#000000"
                shadowOpacity: 0.6; shadowBlur: 100; shadowVerticalOffset: 40
            }
        }

        // Main Glass Panel
        Rectangle {
            id: panel
            anchors.fill: parent
            // Fixed: surfaceVariant didn't exist in Theme.qml, causing it to evaluate to White.
            color: Qt.alpha(Theme.surfaceContainerHighest, 0.8)
            radius: 24
            border.color: Qt.alpha(Theme.outlineVariant, 0.2)
            border.width: 1
            clip: true

            // Inner Glow - Top Left
            Rectangle {
                width: 200; height: 200; radius: 100
                anchors.top: parent.top; anchors.left: parent.left
                anchors.topMargin: -100; anchors.leftMargin: -100
                color: Qt.alpha(Theme.primary, 0.1)
                layer.enabled: true; layer.effect: MultiEffect { blurEnabled: true; blurMax: 60; blur: 1.0 }
            }

            // Inner Glow - Bottom Right
            Rectangle {
                width: 200; height: 200; radius: 100
                anchors.bottom: parent.bottom; anchors.right: parent.right
                anchors.bottomMargin: -100; anchors.rightMargin: -100
                color: Qt.alpha(Theme.secondary, 0.1)
                layer.enabled: true; layer.effect: MultiEffect { blurEnabled: true; blurMax: 60; blur: 1.0 }
            }

            // Bottom Recessed Line
            Rectangle {
                width: parent.width; height: 1
                anchors.bottom: parent.bottom
                gradient: Gradient {
                    orientation: Gradient.Horizontal
                    GradientStop { position: 0.0; color: "transparent" }
                    GradientStop { position: 0.5; color: Qt.alpha(Theme.outlineVariant, 0.3) }
                    GradientStop { position: 1.0; color: "transparent" }
                }
            }
        }
    }

    // --- Content ---
    contentItem: ColumnLayout {
        spacing: 12

        // 1. Neural Orb Animation
        Item {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 160
            Layout.preferredHeight: 160
            Layout.bottomMargin: 32

            // Ping 1 (Outer pulse)
            Rectangle {
                id: ping1
                anchors.centerIn: parent
                width: 128; height: 128; radius: 64
                color: "transparent"
                border.width: 2; border.color: Qt.alpha(Theme.primary, 0.4)
                layer.enabled: true; layer.effect: MultiEffect { blurEnabled: true; blurMax: 1; blur: 1.0 }

                ParallelAnimation {
                    running: root.opened; loops: Animation.Infinite
                    NumberAnimation { target: ping1; property: "scale"; from: 1.0; to: 2.0; duration: 2500; easing.type: Easing.OutQuad }
                    NumberAnimation { target: ping1; property: "opacity"; from: 0.8; to: 0.0; duration: 2500; easing.type: Easing.OutQuad }
                }
            }

            // Ping 2 (Inner pulse with delay)
            Rectangle {
                id: ping2
                anchors.centerIn: parent
                width: 160; height: 160; radius: 80
                color: "transparent"
                border.width: 1; border.color: Qt.alpha(Theme.tertiary, 0.2)
                layer.enabled: true; layer.effect: MultiEffect { blurEnabled: true; blurMax: 2; blur: 1.0 }

                ParallelAnimation {
                    running: root.opened; loops: Animation.Infinite
                    SequentialAnimation {
                        PauseAnimation { duration: 1000 }
                        ParallelAnimation {
                            NumberAnimation { target: ping2; property: "scale"; from: 1.0; to: 2.0; duration: 3500; easing.type: Easing.OutQuad }
                            NumberAnimation { target: ping2; property: "opacity"; from: 0.8; to: 0.0; duration: 3500; easing.type: Easing.OutQuad }
                        }
                    }
                }
            }

            // Dashed Outer Ring (Slow Spin)
            Shape {
                anchors.centerIn: parent
                width: 208; height: 208

                ShapePath {
                    strokeColor: Qt.alpha(Theme.outline, 0.3)
                    fillColor: "transparent"
                    strokeWidth: 0.5
                    strokeStyle: ShapePath.DashLine
                    dashPattern: [8, 8]

                    PathAngleArc {
                        centerX: 104; centerY: 104
                        radiusX: 103; radiusY: 103
                        startAngle: 0; sweepAngle: 360
                    }
                }
                RotationAnimation on rotation { running: root.opened; loops: Animation.Infinite; from: 0; to: 360; duration: 20000 }
            }

            // Solid Outer Ring (Medium Spin)
            Rectangle {
                anchors.centerIn: parent
                width: 160; height: 160; radius: 80
                color: "transparent"
                border.width: 0.5; border.color: Qt.alpha(Theme.primary, 0.2)
                RotationAnimation on rotation { running: root.opened; loops: Animation.Infinite; from: 0; to: 360; duration: 10000 }
            }

            // Solid Inner Ring (Medium Reverse Spin)
            Rectangle {
                anchors.centerIn: parent
                width: 176; height: 176; radius: 88
                color: "transparent"
                border.width: 1; border.color: Qt.alpha(Theme.secondary, 0.1)
                RotationAnimation on rotation { running: root.opened; loops: Animation.Infinite; from: 360; to: 0; duration: 15000 }
            }

            // --- Orbiting Particles ---
            // Slow Outer Orbit (Tertiary)
            Item {
                width: 208; height: 208
                anchors.centerIn: parent
                Rectangle {
                    width: 6; height: 6; radius: 3; color: Theme.tertiary
                    anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter
                    layer.enabled: true; layer.effect: MultiEffect { shadowEnabled: true; shadowColor: Theme.tertiary; shadowBlur: 12 }
                }
                RotationAnimation on rotation { running: root.opened; loops: Animation.Infinite; from: 0; to: 360; duration: 8000 }
            }

            // Medium Reverse Orbit (Secondary)
            Item {
                width: 160; height: 160
                anchors.centerIn: parent
                Rectangle {
                    width: 6; height: 6; radius: 3; color: Theme.secondary
                    anchors.bottom: parent.bottom; anchors.horizontalCenter: parent.horizontalCenter
                    layer.enabled: true; layer.effect: MultiEffect { shadowEnabled: true; shadowColor: Theme.secondary; shadowBlur: 10 }
                }
                RotationAnimation on rotation { running: root.opened; loops: Animation.Infinite; from: 360; to: 0; duration: 5000 }
            }

            // Fast Inner Orbit (Primary)
            Item {
                width: 128; height: 128
                anchors.centerIn: parent
                Rectangle {
                    width: 8; height: 8; radius: 4; color: Theme.primary
                    anchors.top: parent.top; anchors.horizontalCenter: parent.horizontalCenter
                    layer.enabled: true; layer.effect: MultiEffect { shadowEnabled: true; shadowColor: Theme.primary; shadowBlur: 15 }
                }
                RotationAnimation on rotation { running: root.opened; loops: Animation.Infinite; from: 0; to: 360; duration: 3000 }
            }

            // Micro Particle (White)
            Item {
                width: 144; height: 144
                anchors.centerIn: parent
                Rectangle {
                    width: 4; height: 4; radius: 2; color: Qt.alpha("#ffffff", 0.6)
                    anchors.right: parent.right; anchors.top: parent.top; anchors.topMargin: 24
                }
                RotationAnimation on rotation { running: root.opened; loops: Animation.Infinite; from: 360; to: 0; duration: 12000 }
            }

            // --- Central Core ---
            // Core Inner Glow
            Rectangle {
                anchors.centerIn: parent
                width: 40; height: 40; radius: 20
                color: Qt.alpha("#ffffff", 0.2)
                z: 20
                layer.enabled: true; layer.effect: MultiEffect { blurEnabled: true; blurMax: 8; blur: 1.0 }
            }

            // Core Sphere
            Rectangle {
                anchors.centerIn: parent
                width: 56; height: 56; radius: 28
                gradient: Theme.primaryGradient
                z: 10
                layer.enabled: true
                layer.effect: MultiEffect { shadowEnabled: true; shadowColor: Theme.primary; shadowOpacity: 0.7; shadowBlur: 40 }

                SequentialAnimation on scale {
                    running: root.opened; loops: Animation.Infinite
                    NumberAnimation { from: 1.0; to: 1.1; duration: 1000; easing.type: Easing.InOutSine }
                    NumberAnimation { from: 1.1; to: 1.0; duration: 1000; easing.type: Easing.InOutSine }
                }
            }
        }

        // 2. Text Content
        Text {
            Layout.alignment: Qt.AlignHCenter
            visible: root.titleText !== ""
            text: root.titleText
            font.family: Fonts.headlineFamily
            font.pixelSize: 24
            font.bold: true
            font.letterSpacing: -0.5
            color: Theme.textOnSurface
            horizontalAlignment: Text.AlignHCenter
        }

        Text {
            Layout.alignment: Qt.AlignHCenter
            visible: root.descriptionText !== ""
            text: root.descriptionText
            font.family: Fonts.bodyFamily
            font.pixelSize: 14
            color: Theme.textOnSurfaceVariant
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            Layout.maximumWidth: 280
            lineHeight: 1.4
        }
    }
}
