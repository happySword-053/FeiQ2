#pragma once

#include <QWidget>
#include <QVBoxLayout> // 需要包含 QVBoxLayout 的头文件
#include<QList>
class FriendChatWidget : public QWidget
{
    Q_OBJECT // 必须有 Q_OBJECT 宏，因为是 QObject 的子类

public:
    explicit FriendChatWidget(QWidget* parent = nullptr): QWidget(parent) {
        m_vLayout = new QVBoxLayout(this); // 必须初始化布局
        m_vLayout->setAlignment(Qt::AlignTop); // 消息向上对齐
        m_vLayout->setContentsMargins(10, 10, 10, 10); // 设置内边距
        m_vLayout->setSpacing(8); // 设置消息间距
    }
    ~FriendChatWidget();

    // 提供一个公共接口让外部（MainWindow）可以获取到内部的布局，以便添加消息
    QVBoxLayout* getChatLayout() const { return m_vLayout; }

    // 插入MessageBubble 类消息
    void insertMessageBubble(QWidget* messageBubble);
private:
    QVBoxLayout* m_vLayout; // 用于垂直布局的主布局，直接作用于 FriendChatWidget 自身
    QList<QWidget*> m_messageBubbles; // 用于存储消息气泡的列表
};

void FriendChatWidget::insertMessageBubble(QWidget* messageBubble) {
    m_vLayout->addWidget(messageBubble);
    m_messageBubbles.append(messageBubble); // 添加到列表，确保析构时能释放

}
FriendChatWidget::~FriendChatWidget() {
    // 清理消息气泡列表，删除所有消息气泡
    for (QWidget* bubble : m_messageBubbles) {
        delete bubble;
    }
}
