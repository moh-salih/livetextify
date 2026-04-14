#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QDateTime>
#include "liveTextify/modules/session/session.h"


class SessionModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
public:
    enum Roles {
        TitleRole = Qt::UserRole + 1,
        SessionIdRole,
        HasTranscriptionRole,
        HasConversationRole,
        LastActivityRole
    };

    explicit SessionModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addSession(Session* session);
    void removeSession(Session* session);
    void updateSession(Session* session);
    void setCurrentSession(Session* session);
    void clear();


    int indexOf(Session* session) const;
    Session * get(int index) const;

    int count() const;
signals:
    void countChanged();
private slots:
    void onSessionChanged();

private:
    QList<Session*> mSessions;
    Session* mCurrentSession = nullptr;
};
