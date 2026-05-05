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
    function handleLibraryNavigation() {
        Navigator.goToModelLibrary()
    }

    background: Components.PageBackground {
        topColor: Theme.surfaceContainerHighest
        bottomColor: Theme.background
    }

    // --- New Session Dialog ---
    Components.NewSessionDialog {
        id: newSessionDialog
        settings: AppState.sessionSettings // Binds to global defaults for starting

        onStartRequested: function(title) {
            AppState.startNewSession(title)
            newSessionDialog.close()
            Navigator.goToLiveSession("Active Session")
        }

        onResetDefaultsRequested: {
            AppState.resetSessionSettings()
        }
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

                hasModels: root.hasSttModels

                onNewTranscriptionClicked: {
                    newSessionDialog.defaultTitle = "Session " + Qt.formatDateTime(new Date(), "MMM d, hh:mm")
                    newSessionDialog.open()
                }
                onLibraryClicked: root.handleLibraryNavigation()
            }

            Components.LoadingIndicator {
                titleText: "Loading recent sessions..."
                visible: root.deferredSessionModel === null
            }

            Item { Layout.preferredHeight: 40 }
        }
    }
}
