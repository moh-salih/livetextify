#include "liveTextify/modules/session/session.h"
#include "liveTextify/modules/chat/conversation.h"
#include "liveTextify/core/Logger.h"



Session::Session(QObject *parent)
    : QObject(parent)
    , mConversation(new Conversation(this))
{
    // Connect config changes to titleChanged for QML bindings
    connect(this, &Session::configChanged, this, [this]() {
        emit titleChanged();
    });

}

Session::~Session() = default;

void Session::setTranscription(const QString& t) {
    if (mTranscription != t) {
        int prevLength = mTranscription.length();
        int newLength = t.length();

        Logger::debug(QString("Transcription updated: %1 → %2 chars (+%3)").arg(prevLength).arg(newLength).arg(newLength - prevLength));

        mTranscription = t;
        emit transcriptionChanged();
    }
}

void Session::sendMessage(const QString& text){
    Message msg("user", text);
    mConversation->addMessage(msg);
    emit userMessageReady(text, sessionID());
}
