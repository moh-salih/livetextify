#pragma once

#include <QAbstractListModel>
#include <QVector>
#include "liveTextify/modules/chat/Message.h"

class MessageModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum MessageRoles {
        RoleRole = Qt::UserRole + 1,
        ContentRole,
        TimestampRole,
        IsCompleteRole
    };

    explicit MessageModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // CRUD operations
    Q_INVOKABLE void append(const QString &role, const QString &content, bool complete = true);
    Q_INVOKABLE void update(int index, const QString &role, const QString &content, bool complete);
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE void clear();

    // Getters
    Q_INVOKABLE int count() const { return mMessages.size(); }
    Q_INVOKABLE Message get(int index) const;
    Q_INVOKABLE QVector<Message> getAll() const { return mMessages; }

signals:
    void preItemAppended();
    void postItemAppended();
    void preItemRemoved(int index);
    void postItemRemoved();
    void itemUpdated(int index);

private:
    QVector<Message> mMessages;
};
