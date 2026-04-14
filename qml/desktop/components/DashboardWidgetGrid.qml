import QtQuick
import QtQuick.Layouts
import LiveTextify

GridLayout {
    id: root

    columns: root.width > 900 ? 2 : 1
    rowSpacing: 24
    columnSpacing: 24

    property var sessionModel
    signal openSession(int sessionId)

    Repeater {
        model: root.sessionModel

        HistoricalSessionCard {
            Layout.fillWidth: true

            // Data mapped exactly to the SessionModel C++ roles
            title: model.title !== undefined ? model.title : "Unknown Session"
            dateText: model.lastActivity !== undefined ? Qt.formatDateTime(model.lastActivity, "MMM d, yyyy") : "Recently"
            durationText: model.hasTranscription ? "Recorded" : "Empty"
            isRagIndexed: model.hasConversation !== undefined ? model.hasConversation : false

            onDetailsClicked: {
                if (model.sessionId !== undefined) {
                    root.openSession(model.sessionId)
                }
            }
        }
    }
}
