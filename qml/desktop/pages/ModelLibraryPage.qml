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

    // --- Controller Actions ---
    function handleTabClicked(index) {
        root.currentTab = index
    }

    function getActiveManager() {
        if (root.currentTab === 0) return AppState.llama
        if (root.currentTab === 1) return AppState.whisper
        return AppState.embedding
    }

    function handleDownload(index) {
        root.getActiveManager().downloadModel(index)
    }

    function handleCancel(url) {
        root.getActiveManager().cancelDownload(url)
    }

    function handleActivate(index) {
        root.getActiveManager().selectDefaultModel(index)
    }

    function handleDelete(url) {
        root.getActiveManager().deleteModel(url)
    }
    // -------------------------

    background: Rectangle {
        color: Theme.background
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

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 8

                Text {
                    text: "Model Library"
                    font.family: Fonts.headlineFamily
                    font.pixelSize: Fonts.displaySmall
                    font.bold: true
                    color: Theme.textOnSurface
                }
                Text {
                    text: "Download and manage local AI models. One click. Fully offline."
                    font.family: Fonts.bodyFamily
                    font.pixelSize: Fonts.title
                    color: Theme.textOnSurfaceVariant
                    Layout.maximumWidth: 800
                    wrapMode: Text.WordWrap
                }
            }

            GridLayout {
                Layout.fillWidth: true
                columns: root.width > 900 ? 12 : 1
                rowSpacing: 24
                columnSpacing: 24

                // The Below will be removed
                // Components.SystemStatusBento {
                //     Layout.columnSpan: root.width > 900 ? 8 : 1
                //     Layout.fillWidth: true
                //     Layout.fillHeight: true
                //     Layout.minimumHeight: 280
                // }

                // Components.StorageStatsBento {
                //     Layout.columnSpan: root.width > 900 ? 4 : 1
                //     Layout.fillWidth: true
                //     Layout.fillHeight: true
                //     Layout.minimumHeight: 280
                // }
            }

            Components.ModelTypeTabs {
                Layout.alignment: Qt.AlignLeft
                currentTab: root.currentTab
                onTabClicked: (index) => root.handleTabClicked(index)
            }

            Components.ModelListGrid {
                Layout.fillWidth: true
                modelList: root.getActiveManager().listModel

                onDownloadModel: (index) => root.handleDownload(index)
                onCancelDownload: (url) => root.handleCancel(url)
                onActivateModel: (index) => root.handleActivate(index)
                onDeleteModel: (url) => root.handleDelete(url)
            }

            Item { Layout.preferredHeight: 64 }
        }
    }
}
