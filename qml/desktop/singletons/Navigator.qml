// Navigator.qml
pragma Singleton
import QtQuick
import QtQuick.Controls

QtObject {
    property StackView stack: null
    property string currentTitle: "Dashboard"


    function push(page){
        stack.replace(page);
    }

    function goToProfile() {
        currentTitle = "Profile"
        stack.replace(Qt.resolvedUrl("../pages/ProfilePage.qml"))
    }

    function goToDashboard() {
        currentTitle = "Dashboard"
        stack.replace(Qt.resolvedUrl("../pages/DashboardPage.qml"))
    }

    function goToSessions() {
        currentTitle = "Sessions"
        stack.replace(Qt.resolvedUrl("../pages/SessionPage.qml"))
    }

    function goToLiveSession(title) {
        currentTitle = title ?? "Active Session"
        stack.replace(Qt.resolvedUrl("../pages/LiveSessionPage.qml"))
    }

    function goToModelLibrary() {
        currentTitle = "Model Library"
        stack.replace(Qt.resolvedUrl("../pages/ModelLibraryPage.qml"))
    }

    function goToSettings() {
        currentTitle = "Settings"
        stack.replace(Qt.resolvedUrl("../pages/SettingsPage.qml"))
    }
}
