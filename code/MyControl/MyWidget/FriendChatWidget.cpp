#include "FriendChatWidget.h"

FriendChatWidget::FriendChatWidget(QWidget* parent) : QWidget(parent) {
    m_vLayout = new QVBoxLayout(this);
    m_vLayout->setAlignment(Qt::AlignTop);
    m_vLayout->setContentsMargins(10, 10, 10, 10);
    m_vLayout->setSpacing(8);
}

FriendChatWidget::~FriendChatWidget() {
    for (QWidget* bubble : m_messageBubbles) {
        delete bubble;
    }
}

QVBoxLayout* FriendChatWidget::getChatLayout() const {
    return m_vLayout;
}

void FriendChatWidget::insertMessageBubble(QWidget* messageBubble) {
    m_vLayout->addWidget(messageBubble);
    m_messageBubbles.append(messageBubble);
}