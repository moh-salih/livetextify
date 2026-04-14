#include "liveTextify/modules/models/modellistmodel.h"
#include <QUrl>
#include <QFileInfo>

ModelListModel::ModelListModel(QObject *parent)
    : QAbstractListModel(parent) {
}

int ModelListModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return m_items.count();
}

QVariant ModelListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_items.count())
        return QVariant();

    const ModelItem &item = m_items.at(index.row());

    switch (role) {
    case FileNameRole: return item.fileName;
    case UrlRole: return item.url;
    case SizeRole: return item.size;
    case DownloadedRole: return item.downloaded;
    case DownloadingRole: return item.downloading;
    case ProgressRole: return item.progress;
    case IsDefaultRole: return item.isDefault;
    default: return QVariant();
    }
}

QHash<int, QByteArray> ModelListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[FileNameRole] = "fileName";
    roles[UrlRole] = "url";
    roles[SizeRole] = "size";
    roles[DownloadedRole] = "downloaded";
    roles[DownloadingRole] = "downloading";
    roles[ProgressRole] = "progress";
    roles[IsDefaultRole] = "isDefault";
    return roles;
}

void ModelListModel::populate(const QVector<ModelItem> &items) {
    beginResetModel();
    m_items = items;
    endResetModel();
}

void ModelListModel::updateProgress(int row, float progress) {
    if (row < 0 || row >= m_items.count()) return;
    m_items[row].progress = progress;
    QModelIndex idx = index(row);
    emit dataChanged(idx, idx, {ProgressRole});
}

void ModelListModel::setDownloadingStatus(int row, bool downloading) {
    if (row < 0 || row >= m_items.count()) return;
    m_items[row].downloading = downloading;
    QModelIndex idx = index(row);
    emit dataChanged(idx, idx, {DownloadingRole});
}

void ModelListModel::setDownloadedStatus(int row, bool downloaded) {
    if (row < 0 || row >= m_items.count()) return;
    m_items[row].downloaded = downloaded;
    QModelIndex idx = index(row);
    emit dataChanged(idx, idx, {DownloadedRole});
}

void ModelListModel::setExclusiveDefault(int row) {
    for (int i = 0; i < m_items.count(); ++i) {
        bool newVal = (i == row);
        if (m_items[i].isDefault != newVal) {
            m_items[i].isDefault = newVal;
            QModelIndex idx = index(i);
            emit dataChanged(idx, idx, {IsDefaultRole});
        }
    }
}

const ModelItem& ModelListModel::getItem(int row) const {
    static ModelItem empty;
    if (row < 0 || row >= m_items.count()) return empty;
    return m_items.at(row);
}

const QVector<ModelItem>& ModelListModel::getAll() const{
    return m_items;
}

int ModelListModel::findIndexByPath(const QString &fullPath, const QString &baseDir) const {
    QString targetFile = QFileInfo(fullPath).fileName();
    for (int i = 0; i < m_items.count(); ++i) {
        QString itemFile = QUrl(m_items[i].url).fileName();
        if (itemFile == targetFile) return i;
    }
    return -1;
}
