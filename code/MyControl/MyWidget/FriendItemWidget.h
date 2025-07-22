#pragma once
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include"../../infoModule/FriendInfo.h"
// 好友项控件，用于显示好友头像、名称和未读消息数量
class FriendItemWidget : public QWidget {
    Q_OBJECT

public:
    // 构造函数，初始化好友项，包括头像、名称和未读消息徽章
    explicit FriendItemWidget(const QString &name, QWidget *parent = nullptr);
    explicit FriendItemWidget(const FriendInfo &friendInfo, QWidget *parent = nullptr);
    // 获取好友名称
    QString getName() const { return m_name; }

    // 增加未读数量，并更新气泡显示
    void incrementUnread() {
        m_unreadCount++;
        updateBadge(); // 更新徽章显示
    }

    // 清空未读数量，并隐藏气泡
    void clearUnread() {
        m_unreadCount = 0;
        updateBadge(); // 更新徽章显示
    }
    FriendInfo getFriendInfo(){
        return m_friendInfo;
    }
private:
    // 更新未读消息徽章的显示状态和文本
    void updateBadge() {
        if (m_unreadCount > 0) {
            m_badge->setText(QString::number(m_unreadCount)); // 设置未读数量文本
            m_badge->setVisible(true); // 显示徽章
        } else {
            m_badge->setVisible(false); // 隐藏徽章
        }
    }
    void init(); 
    QString m_name;        // 好友名称
    int m_unreadCount;     // 未读消息数量
    QLabel *m_icon;        // 头像标签
    QLabel *m_labelName;   // 名称标签
    QLabel *m_badge;       // 未读消息徽章标签
    FriendInfo m_friendInfo; // 好友信息
};