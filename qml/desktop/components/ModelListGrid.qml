import QtQuick
import QtQuick.Layouts
import LiveTextify

ColumnLayout {
    id: root
    spacing: 16

    property var modelList

    // Propagate signals up to the Smart Page
    signal downloadModel(int index)
    signal cancelDownload(string url)
    signal activateModel(int index)
    signal deleteModel(string url)

    Repeater {
        model: root.modelList

        ModelListCard {
            // Map C++ roles to QML properties
            modelName: model.fileName !== undefined ? model.fileName : ""
            fileSize: model.size !== undefined ? model.size : ""
            downloadProgress: model.progress !== undefined ? model.progress : 0.0

            // Derive UI state from C++ flags
            status: {
                if (model.isDefault) return "default"
                if (model.downloading) return "downloading"
                if (model.downloaded) return "downloaded"
                return "available"
            }

            // Route card clicks up to the Grid
            onDownloadClicked: root.downloadModel(index)
            onCancelClicked: root.cancelDownload(model.url)
            onActivateClicked: root.activateModel(index)
            onDeleteClicked: root.deleteModel(model.url)
        }
    }
}
