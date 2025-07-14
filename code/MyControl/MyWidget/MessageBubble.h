#pragma once
/*
聊天消息气泡
*/
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QDateTime>

// 消息类型枚举
enum class MessageType {
    Incoming,  // 接收消息（左对齐，黄色气泡）
    Outgoing   // 发送消息（右对齐，蓝色气泡）
};
class MessageBubble : public QWidget {
    Q_OBJECT

public:
    // 构造函数：接收消息类型、发送者名称、消息内容
    explicit MessageBubble(MessageType type, const QString& sender, const QString& time,
                              const QString& content, QWidget *parent = nullptr);

private:
    // 创建头部区域（时间+发送者）
    QWidget* createHeader(const QString& timeStr, const QString& sender, MessageType type);

    // 创建气泡内容区域
    QWidget* createBubble(const QString& content, MessageType type);

    QVBoxLayout* m_mainLayout;  // 主布局
};
MessageBubble::MessageBubble(MessageType type, const QString& sender, const QString& time,
                             const QString& content, QWidget *parent) : QWidget(parent) {
    // 设置主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 12);  // 底部留出12px间距
    m_mainLayout->setSpacing(4);

    
    // 添加头部和气泡到主布局
    m_mainLayout->addWidget(createHeader(time, sender, type));
    m_mainLayout->addWidget(createBubble(content, type));                                 
}
QWidget* MessageBubble::createHeader(const QString& timeStr, const QString& sender, MessageType type) {
    QWidget* headerContainer = new QWidget;
    QHBoxLayout* headerHL = new QHBoxLayout(headerContainer);
    headerHL->setContentsMargins(0, 0, 0, 0);
    headerHL->setSpacing(0);

    QString headerText;
    if (type == MessageType::Outgoing) {
        headerText = QString("[%1] Me → %2").arg(timeStr).arg(sender);
    } else {
        headerText = QString("[%1] %2").arg(timeStr).arg(sender);
    }

    QLabel* header = new QLabel(headerText);
    header->setStyleSheet("color: #555; font-size:12px;");

    // 根据消息类型设置对齐方式
    if (type == MessageType::Outgoing) {
        headerHL->addStretch(1);  // 右对齐
        headerHL->addWidget(header);
    } else {
        headerHL->addWidget(header);
        headerHL->addStretch(1);  // 左对齐
    }

    return headerContainer;
}

QWidget* MessageBubble::createBubble(const QString& content, MessageType type) {
    QWidget* bubbleContainer = new QWidget;
    QHBoxLayout* bubbleHL = new QHBoxLayout(bubbleContainer);
    bubbleHL->setContentsMargins(0, 0, 0, 0);
    bubbleHL->setSpacing(0);

    QLabel* bubble = new QLabel(content);
    bubble->setWordWrap(true);
    bubble->setStyleSheet(
        type == MessageType::Outgoing ?
        "background:#d0eaff; padding:6px; border-radius:4px; font-size:14px;" :
        "background:#fff1a8; padding:6px; border-radius:4px; font-size:14px;"
    );

    // 根据消息类型设置对齐方式
    if (type == MessageType::Outgoing) {
        bubbleHL->addStretch(1);  // 右对齐
        bubbleHL->addWidget(bubble);
    } else {
        bubbleHL->addWidget(bubble);
        bubbleHL->addStretch(1);  // 左对齐
    }

    return bubbleContainer;
}