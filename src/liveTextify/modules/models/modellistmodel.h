#pragma once
#include <QAbstractListModel>
#include <QVector>

struct ModelItem {
    QString fileName;
    QString url;
    QString size;
    bool downloaded = false;
    bool downloading = false;
    float progress = 0.0f;
    bool isDefault = false;
};

class ModelListModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum ModelRoles {FileNameRole = Qt::UserRole + 1, UrlRole, SizeRole, DownloadedRole, DownloadingRole, ProgressRole, IsDefaultRole };

    explicit ModelListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void populate(const QVector<ModelItem> &items);
    void updateProgress(int row, float progress);
    void setDownloadingStatus(int row, bool downloading);
    void setDownloadedStatus(int row, bool downloaded);
    void setExclusiveDefault(int row);

    const ModelItem& getItem(int row) const;
    const QVector<ModelItem>& getAll() const;
    int findIndexByPath(const QString &fullPath, const QString &baseDir) const;

private:
    QVector<ModelItem> m_items;
};
