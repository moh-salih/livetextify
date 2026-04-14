#include "liveTextify/modules/session/sessionmodel.h"
#include <QDebug>


SessionModel::SessionModel(QObject *parent)
    : QAbstractListModel(parent) {}

int SessionModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return mSessions.count();
}

QVariant SessionModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= mSessions.count())
        return QVariant();

    Session* session = mSessions.at(index.row());

    switch (role) {
    case TitleRole:
        return session->title();
    case SessionIdRole:
        return session->sessionID();
    case HasTranscriptionRole:
        return !session->transcription().isEmpty();
    case HasConversationRole:
        return session->conversation() != nullptr;
    case LastActivityRole:
        // You might want to track last activity in Session
        return QDateTime::currentDateTime();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> SessionModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[SessionIdRole] = "sessionId";
    roles[HasTranscriptionRole] = "hasTranscription";
    roles[HasConversationRole] = "hasConversation";
    roles[LastActivityRole] = "lastActivity";
    return roles;
}

void SessionModel::addSession(Session* session) {
    beginInsertRows(QModelIndex(), mSessions.size(), mSessions.size());
    mSessions.append(session);
    connect(session, &Session::titleChanged, this, &SessionModel::onSessionChanged);
    endInsertRows();

    emit countChanged();
}

void SessionModel::removeSession(Session* session) {
    int index = mSessions.indexOf(session);
    if (index < 0) return;

    beginRemoveRows(QModelIndex(), index, index);
    mSessions.removeAt(index);
    endRemoveRows();

    if (session == mCurrentSession) {
        mCurrentSession = nullptr;
    }

    emit countChanged();
}

void SessionModel::updateSession(Session* session) {
    int index = mSessions.indexOf(session);
    if (index < 0) return;

    QModelIndex modelIndex = this->index(index);
    emit dataChanged(modelIndex, modelIndex, {TitleRole});
}
void SessionModel::setCurrentSession(Session* session) {
    // Clear old active flag (runtime only)
    if (mCurrentSession) {
        int oldIndex = mSessions.indexOf(mCurrentSession);
        if (oldIndex >= 0) {
            QModelIndex oldIdx = this->index(oldIndex);
            // We still need to notify UI that this session is no longer active
            // But since we removed IsActiveRole, UI will need to compare with mCurrentSession
            emit dataChanged(oldIdx, oldIdx, {});  // Empty role list means all roles
        }
    }

    mCurrentSession = session;

    // Set new active flag
    if (session) {
        int newIndex = mSessions.indexOf(session);
        if (newIndex >= 0) {
            QModelIndex newIdx = this->index(newIndex);
            emit dataChanged(newIdx, newIdx, {});
        }
    }
}


void SessionModel::onSessionChanged() {
    Session* session = qobject_cast<Session*>(sender());
    if (session) {
        updateSession(session);
    }
}

void SessionModel::clear() {
    beginResetModel();
    mSessions.clear();
    mCurrentSession = nullptr;
    endResetModel();

    emit countChanged();
}

int SessionModel::indexOf(Session* session) const {
    return mSessions.indexOf(session);
}

Session * SessionModel::get(int index) const {
    if (index < 0 || index >= mSessions.count())
        return nullptr;
    return mSessions.at(index);
}

int SessionModel::count() const {
    return mSessions.count();
}


