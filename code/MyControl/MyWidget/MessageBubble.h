#pragma once
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

enum class MessageType { Incoming, Outgoing };

class MessageBubble : public QWidget {
    Q_OBJECT
public:
    explicit MessageBubble(MessageType type, const QString& sender, const QString& time,
                          const QString& content, QWidget *parent = nullptr);
private:
    QWidget* createHeader(const QString& timeStr, const QString& sender, MessageType type);
    QWidget* createBubble(const QString& content, MessageType type);
    QVBoxLayout* m_mainLayout;
};
// 注意：此处删除所有函数实现代码