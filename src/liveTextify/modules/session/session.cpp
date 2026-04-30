// session.cpp
#include "session.h"
#include "liveTextify/modules/chat/conversation.h"

Session::Session(QObject* parent) : QObject(parent) {}

void Session::setConfig(const SessionConfig& cfg) {
    mConfig = cfg;
}
void Session::loadConversation(const QVector<Message>& messages) {
    if (!mConversation)
        mConversation = new Conversation(this);
    mConversation->clear();
    for (const auto& msg : messages)
        mConversation->addMessage(msg);
    emit conversationLoaded();
}

void Session::unloadConversation() {
    delete mConversation;
    mConversation = nullptr;
}

void Session::loadTranscription(const QString& text) {
    mTranscription = text;
    emit transcriptionChanged();
}

void Session::unloadTranscription() {
    mTranscription.clear();
}
