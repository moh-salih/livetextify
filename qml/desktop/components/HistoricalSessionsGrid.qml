import QtQuick
import QtQuick.Layouts
import LiveTextify

GridLayout {
    id: root

    // Responsive columns based on width
    columns: root.width > 900 ? 2 : 1
    rowSpacing: 24
    columnSpacing: 24

    // Bound to C++ SessionModel
    property var sessionModel

    // Propagate action up to the Page
    signal openSession(int sessionId)

    // Automatically generates a card for every item in the C++ model
    Repeater {
        model: root.sessionModel

        HistoricalSessionCard {
            Layout.fillWidth: true

            // Map C++ roles defined in SessionModel::roleNames()
            title: model.title !== undefined ? model.title : "Unknown Session"

            // Format the CDateTime exposed by C++ (if added to roles), or fallback
            // Note: Update your SessionModel to expose createdAt if you want real dates here
            dateText: model.lastActivity !== undefined ? Qt.formatDateTime(model.lastActivity, "MMM d, yyyy") : "Recently"

            durationText: model.hasTranscription ? "Recorded" : "Empty"
            isRagIndexed: model.hasConversation !== undefined ? model.hasConversation : false

            // Emit the specific session ID to the parent grid
            onDetailsClicked: {
                if (model.sessionId !== undefined) {
                    root.openSession(model.sessionId)
                }
            }
        }
    }
}
