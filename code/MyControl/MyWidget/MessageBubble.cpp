#include "MessageBubble.h"

MessageBubble::MessageBubble(MessageType type, const QString& sender, const QString& time,
                           const QString& content, QWidget *parent) : QWidget(parent) {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 12);
    m_mainLayout->setSpacing(4);
    m_mainLayout->addWidget(createHeader(time, sender, type));
    m_mainLayout->addWidget(createBubble(content, type));
}

QWidget* MessageBubble::createHeader(const QString& timeStr, const QString& sender, MessageType type) {
    // 实现代码...
}

QWidget* MessageBubble::createBubble(const QString& content, MessageType type) {
    // 实现代码...
}