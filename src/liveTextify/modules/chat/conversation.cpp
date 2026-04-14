#include "conversation.h"

Conversation::Conversation(QObject *parent)
    : QObject(parent)
    , mConversationID(-1)
    , mMessageModel(new MessageModel(this))
{
}

Conversation::~Conversation() = default;

void Conversation::addMessage(const Message& message) {
    mMessageModel->append(message.role, message.content, message.isComplete);
}
void Conversation::addMessage(const QString& role, const QString& message, bool isComplete){
    mMessageModel->append(role, message, isComplete);
}
void Conversation::updateMessage(int index, const Message& message) {
    if (index < 0 || index >= mMessageModel->count()) return;
    mMessageModel->update(index, message.role, message.content, message.isComplete);
}

void Conversation::removeMessage(int index) {
    if (index < 0 || index >= mMessageModel->count()) return;
    mMessageModel->remove(index);
}

void Conversation::clear() {
    mMessageModel->clear();
}
