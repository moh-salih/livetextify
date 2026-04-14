#pragma once

#include <QtCore>
#include "liveTextify/modules/chat/messagemodel.h"

class Conversation : public QObject {
    Q_OBJECT
    Q_PROPERTY(int conversationID READ conversationID CONSTANT)
    Q_PROPERTY(MessageModel* messages READ messages CONSTANT)

public:
    explicit Conversation(QObject *parent = nullptr);
    ~Conversation();


    // Setters
    void setConversationID(int id){ mConversationID = id; }
    void setTitle(const QString& title){ mTitle = title; }

    // Getters
    int conversationID() const { return mConversationID; }
    QString title() const { return mTitle; }

    MessageModel* messages() const { return mMessageModel; }

    // Pure CRUD operations on messages
    Q_INVOKABLE void addMessage(const QString& role, const QString& message, bool isComplete = true);
    Q_INVOKABLE void addMessage(const Message& message);
    Q_INVOKABLE void updateMessage(int index, const Message& message);
    Q_INVOKABLE void removeMessage(int index);
    Q_INVOKABLE void clear();

    // No signals needed - MessageModel already has its own signals

private:
    int         mConversationID;
    QString     mTitle;

    MessageModel* mMessageModel;
};
