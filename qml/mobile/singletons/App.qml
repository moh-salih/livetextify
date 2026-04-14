pragma Singleton
import QtQuick
import QtCore

Settings {
    id: root
    category: "Global"

    // Appearance
    property bool isDarkMode: false
    property color primaryColor: "#2962FF"

    // Transcription (STT)
    property int sttLanguageIndex: 0
    property int vadSensitivity: 75
    property bool autoTranslate: false
    property int sttThreads: 4
    property bool sttAutoPunctuation: true

    // Intelligence (LLM)
    property real temperature: 0.7
    property int contextLimitIndex: 1
    property real topP: 0.9
    property real repeatPenalty: 1.1

    // Knowledge (RAG)
    property real similarityThreshold: 0.35
    property int topK: 5
    property int chunkOverlap: 50

    // Storage & Sessions
    property int autoSaveIndex: 0
    property bool confirmDelete: true

    // Font Persistence
    property string fontFamily: "Iosevka Charon"
    property real fontScale: 1.0
}
