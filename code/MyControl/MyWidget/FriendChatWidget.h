#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QList>

class FriendChatWidget : public QWidget {
    Q_OBJECT
public:
    explicit FriendChatWidget(QWidget* parent = nullptr);
    ~FriendChatWidget();
    QVBoxLayout* getChatLayout() const; 
    void insertMessageBubble(QWidget* messageBubble);
private:
    QVBoxLayout* m_vLayout;
    QList<QWidget*> m_messageBubbles;
};
