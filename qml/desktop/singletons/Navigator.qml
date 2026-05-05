pragma Singleton
import QtQuick
import QtQuick.Controls

QtObject {
    property StackView stack: null
    property string currentTitle: "Dashboard"

    function push(page){
        stack.push(page);
    }

    function goBack() {
        if (stack && stack.depth > 1) {
            stack.pop()
        }
    }

    function goToProfile() {
        currentTitle = "Profile"
        stack.push(Qt.resolvedUrl("../pages/ProfilePage.qml"))
    }

    function goToDashboard() {
        currentTitle = "Dashboard"
        // pop(null) pops all items down to the initial root item (Dashboard)
        if (stack) stack.pop(null);
    }

    function goToSessions() {
        currentTitle = "Sessions"
        stack.push(Qt.resolvedUrl("../pages/SessionPage.qml"))
    }

    function goToLiveSession(title) {
        currentTitle = title ?? "Active Session"
        stack.push(Qt.resolvedUrl("../pages/LiveSessionPage.qml"))
    }

    function goToModelLibrary() {
        currentTitle = "Model Library"
        stack.push(Qt.resolvedUrl("../pages/ModelLibraryPage.qml"))
    }

    function goToSettings() {
        currentTitle = "Settings"
        stack.push(Qt.resolvedUrl("../pages/SettingsPage.qml"))
    }
}
