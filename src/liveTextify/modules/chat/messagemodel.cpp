#include "messagemodel.h"

MessageModel::MessageModel(QObject *parent)
    : QAbstractListModel(parent) {}

int MessageModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return mMessages.count();
}

QVariant MessageModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= mMessages.count())
        return QVariant();

    const Message &msg = mMessages.at(index.row());

    switch (role) {
    case RoleRole: return msg.role;
    case ContentRole: return msg.content;
    case TimestampRole: return msg.timestamp;
    case IsCompleteRole: return msg.isComplete;
    default: return QVariant();
    }
}

QHash<int, QByteArray> MessageModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[RoleRole] = "role";
    roles[ContentRole] = "content";
    roles[TimestampRole] = "timestamp";
    roles[IsCompleteRole] = "isComplete";
    return roles;
}

void MessageModel::append(const QString &role, const QString &content, bool complete) {
    emit preItemAppended();
    beginInsertRows(QModelIndex(), mMessages.size(), mMessages.size());
    mMessages.append(Message(role, content, complete));
    endInsertRows();
    emit postItemAppended();
}

void MessageModel::update(int index, const QString &role, const QString &content, bool complete) {
    if (index < 0 || index >= mMessages.size()) return;

    mMessages[index].role = role;
    mMessages[index].content = content;
    mMessages[index].isComplete = complete;
    // timestamp could be updated or left as original

    QModelIndex modelIndex = this->index(index);
    emit dataChanged(modelIndex, modelIndex, {RoleRole, ContentRole, IsCompleteRole});
    emit itemUpdated(index);
}

void MessageModel::remove(int index) {
    if (index < 0 || index >= mMessages.size()) return;

    emit preItemRemoved(index);
    beginRemoveRows(QModelIndex(), index, index);
    mMessages.removeAt(index);
    endRemoveRows();
    emit postItemRemoved();
}

void MessageModel::clear() {
    beginResetModel();
    mMessages.clear();
    endResetModel();
}

Message MessageModel::get(int index) const {
    if (index < 0 || index >= mMessages.size())
        return Message();
    return mMessages.at(index);
}
