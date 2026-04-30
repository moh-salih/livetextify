pragma Singleton
import QtQuick

QtObject {
    // --- Global UI State ---
    property bool isClearMode: false

    // Surface & Backgrounds
    readonly property color background: "#060e20"
    readonly property color surface: "#060e20"
    readonly property color surfaceContainerLowest: "#000000"
    readonly property color surfaceContainerLow: "#091328"
    readonly property color surfaceContainer: "#0f1930"
    readonly property color surfaceContainerHigh: "#141f38"
    readonly property color surfaceContainerHighest: "#192540"

    // Primary
    readonly property color primary: "#8ff5ff"
    readonly property color textOnPrimary: "#005d63"

    // Secondary
    readonly property color secondary: "#bf81ff"
    readonly property color textOnSecondary: "#32005c"

    // Tertiary
    readonly property color tertiary: "#65afff"
    readonly property color textOnTertiary: "#002e52"

    // Text & Outlines
    readonly property color textOnSurface: "#dee5ff"
    readonly property color textOnSurfaceVariant: "#a3aac4"
    readonly property color outline: "#6d758c"
    readonly property color outlineVariant: "#40485d"

    readonly property color errorColor: "#ff716c"

    // Gradients
    readonly property Gradient primaryGradient: Gradient {
        orientation: Gradient.Horizontal

        GradientStop {
            position: 0.0
            color: "#8ff5ff"
        }

        GradientStop {
            position: 1.0
            color: "#65afff"
        }
    }
}
