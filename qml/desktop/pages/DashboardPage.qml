import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify
import "../components" as Components
import "../singletons"

Page {
    id: root

    // --- Deferred Loading State ---
    property var deferredSessionModel: null
    property bool hasSttModels: false

    Timer {
        id: loadTimer
        interval: 200
        running: true
        onTriggered: root.deferredSessionModel = AppState.sessionSvc.sessions
    }

    // --- Hidden Tracker to evaluate downloaded models ---
    Repeater {
        id: sttModelTracker
        model: AppState.sttModel.models
        delegate: Item {
            property bool isDownloaded: model.downloaded === true
            onIsDownloadedChanged: Qt.callLater(root.evaluateModelAvailability)
            Component.onCompleted: Qt.callLater(root.evaluateModelAvailability)
        }
    }

    function evaluateModelAvailability() {
        let found = false;
        for (let i = 0; i < sttModelTracker.count; i++) {
            let item = sttModelTracker.itemAt(i);
            if (item && item.isDownloaded) {
                found = true;
                break;
            }
        }
        root.hasSttModels = found;
    }

    // --- Actions ---
    function handleNewTranscription() {
        AppState.startNewSession()
        Navigator.goToLiveSession("Active Session")
    }

    function handleLibraryNavigation() {
        Navigator.goToModelLibrary()
    }

    background: Components.PageBackground {
        topColor: Theme.surfaceContainerHighest
        bottomColor: Theme.background
    }

    ScrollView {
        anchors.fill: parent
        clip: true
        contentWidth: availableWidth

        ColumnLayout {
            width: Math.min(1200, parent.width - 48)
            anchors.horizontalCenter: parent.horizontalCenter
            y: 40
            spacing: 40

            Components.DashboardHero {
                Layout.fillWidth: true

                // Pass evaluated state down
                hasModels: root.hasSttModels

                // Handle upward signals
                onNewTranscriptionClicked: root.handleNewTranscription()
                onLibraryClicked: root.handleLibraryNavigation()
            }

            Components.LoadingIndicator {
                titleText: "Loading recent sessions..."
                visible: root.deferredSessionModel === null
            }

            // Components.DashboardWidgetGrid {
            //     Layout.fillWidth: true
            //     visible: root.deferredSessionModel !== null
            //     sessionModel: root.deferredSessionModel
            // }

            Item { Layout.preferredHeight: 40 }
        }
    }
}
