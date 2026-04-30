import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LiveTextify
import "../singletons"
import "../components" as Components

Page {
    id: root

    // --- State Properties ---
    property int currentTab: 0

    readonly property var currentSubMgr: {
        if (currentTab === 0) return AppState.llmModel
        if (currentTab === 1) return AppState.sttModel
        return AppState.embModel
    }

    readonly property var currentListModel: currentSubMgr ? currentSubMgr.models : null

    function handleDownload(index)  { AppState.downloadModel(currentSubMgr, index) }
    function handleCancel(url)      { AppState.cancelDownload(currentSubMgr, url) }
    function handleActivate(index)  { AppState.selectModel(currentSubMgr, index) }
    function handleDelete(url)      { AppState.deleteModel(currentSubMgr, url) }

    // --- Controller Actions ---
    function handleTabClicked(index) {
        root.currentTab = index
    }

    // -------------------------

    background: Components.PageBackground {
        topColor: Theme.background
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

            Components.PageHeader {
                Layout.fillWidth: true
                title: "Model Library"
                description: "Download and manage local AI models. One click. Fully offline."
            }

            GridLayout {
                Layout.fillWidth: true
                columns: root.width > 900 ? 12 : 1
                rowSpacing: 24
                columnSpacing: 24
            }

            Components.ModelTypeTabs {
                Layout.alignment: Qt.AlignLeft
                currentTab: root.currentTab
                onTabClicked: (index) => root.handleTabClicked(index)
            }

            Components.ModelListGrid {
                Layout.fillWidth: true
                modelList: root.currentListModel

                onDownloadModel: (index) => root.handleDownload(index)
                onCancelDownload: (url) => root.handleCancel(url)
                onActivateModel: (index) => root.handleActivate(index)
                onDeleteModel: (url) => root.handleDelete(url)
            }

            Item { Layout.preferredHeight: 64 }
        }
    }
}
