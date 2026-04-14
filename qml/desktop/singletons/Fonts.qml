pragma Singleton
import QtQuick

QtObject {
    // --- Font Families ---
    readonly property string bodyFamily: "Manrope"
    readonly property string headlineFamily: "Space Grotesk"
    readonly property string iconFamily: "Material Symbols Outlined"

    readonly property string sharpIcon:   "Material Symbols Sharp"
    readonly property string roundedIcon: "Material Symbols Rounded"
    readonly property string outlinedIcon:"Material Symbols Outlined"

    // --- Font Sizes (Semantic) ---
    readonly property int micro: 9
    readonly property int tiny: 10
    readonly property int small: 12
    readonly property int normal: 14
    readonly property int large: 16
    readonly property int title: 18
    readonly property int heading: 20
    readonly property int largeTitle: 24
    readonly property int displaySmall: 36
    readonly property int display: 48
}
