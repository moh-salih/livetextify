pragma Singleton
import QtQuick

QtObject {
    readonly property bool isDark: App.isDarkMode

    // ========== UTILITY COLORS ==========
    readonly property color primaryColor: App.primaryColor
    readonly property color successColor: "#4CAF50"
    readonly property color warningColor: "#FFC107"
    readonly property color errorColor: "#E53935"


    // ========== BACKGROUNDS ==========
    readonly property color background: isDark ? "#121212" : "#FFFFFF"
    readonly property color surfaceColor: isDark ? "#1E1E1E" : "#F6F6F6"
    readonly property color dialogBackground: surfaceColor

    // ========== TEXT COLORS ==========
    readonly property color textPrimary: isDark ? "#FFFFFF" : "#222222"
    readonly property color textSecondary: isDark ? "#CCCCCC" : "#555555"
    readonly property color textHint: isDark ? "#777777" : "#999999"
    readonly property color textOnPrimary: "white"

    // ========== BORDERS & SEPARATORS ==========
    readonly property color borderColor: isDark ? "#333" : "#D9D9D9"
    readonly property color dialogBorder: borderColor
    readonly property color separator: borderColor


    // ========== BADGES ==========
    readonly property color badgeRed: isDark ? "#B71C1C" : "#e74c3c"
    readonly property color badgeBlue: isDark ? "#0D47A1" : "#3498db"

    // ========== CHAT BUBBLES ==========
    readonly property color userBubbleColor: isDark ? "#2962FF" : "#4A90E2"
    readonly property color userTextColor: "white"
    readonly property color assistantBubbleColor: isDark ? "#2A2A2A" : "#EEEEEE"
    readonly property color assistantTextColor: isDark ? "#FAFAFA" : "#222222"

    // ========== INPUT FIELDS ==========
    readonly property color inputBackground: isDark ? "#1F1F1F" : "#EEE"
    readonly property color inputText: textPrimary
    readonly property color inputPlaceholder: textHint
    readonly property color scrollbarColor: isDark ? "#444" : "#C3C3C3"

    // ========== BUTTONS ==========
    readonly property color sendButtonActive: successColor
    readonly property color sendButtonDisabled: isDark ? "#444" : "#DDD"
    readonly property color sendButtonIcon: "#FFF"

    // ========== ICONS ==========
    readonly property color iconDefault: isDark ? "#FFF" : "#555"

    // ========== STATES ==========
    readonly property color pressedOverlay: isDark ? "#FFFFFF" : "#000000"
    readonly property real pressedOpacity: 0.1
    readonly property color disabledBackground: isDark ? "#2A2A2A" : "#F0F0F0"
    readonly property color disabledText: textHint

    // ========== SHADOWS ==========
    readonly property color shadowColor: "#000000"
    readonly property real shadowOpacity: isDark ? 0.5 : 0.1

    // ========== DIMENSIONS ==========
    readonly property int radiusSmall: 4
    readonly property int radiusMedium: 8
    readonly property int radiusLarge: 16
    readonly property int radiusXLarge: 24

    readonly property int spacingSmall: 4
    readonly property int spacingMedium: 8
    readonly property int spacingLarge: 16
    readonly property int spacingXLarge: 24

    // ========== TYPOGRAPHY ==========
    readonly property string fontFamily: App.fontFamily
    readonly property real fontScale: App.fontScale

    // ========== DIMENSIONS (Scaled) ==========
    readonly property int fontSizeXSmall: 8 * fontScale
    readonly property int fontSizeSmall: 12 * fontScale
    readonly property int fontSizeMedium: 16 * fontScale
    readonly property int fontSizeLarge: 20 * fontScale
    readonly property int fontSizeXLarge: 24 * fontScale

    // Images
    readonly property string appImage: "qrc:/icons/app.png"
}
