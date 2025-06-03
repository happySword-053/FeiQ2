#pragma once
#include<qwidget.h>
#include<qlabel.h>
#include<QHBoxLayout>
// 用来自定义“好友列表”中每一行的 Widget
class FriendItemWidget : public QWidget {
    Q_OBJECT

public:
    // 构造函数，初始化好友项，包括头像、名称和未读消息徽章
    explicit FriendItemWidget(const QString &name, QWidget *parent = nullptr)
        : QWidget(parent), m_name(name), m_unreadCount(0)
    {
        // 1) 头像 Label：灰色圆底，并在中间显示用户名首字母/汉字
        m_icon = new QLabel(this);
        m_icon->setFixedSize(32, 32); // 设置固定大小
        m_icon->setAlignment(Qt::AlignCenter); // 文本居中对齐
        m_icon->setStyleSheet( // 设置样式表
            "background: gray; "
            "color: white; "
            "font-weight: bold; "
            "border-radius: 16px; " // 圆角，使其成为圆形
            "font-size: 14px;"
            );

        // 取名字的第一个字符（支持英文或汉字）作为头像显示
        if (!name.isEmpty()) {
            m_icon->setText(name.left(1));
        }

        // 2) 名称 Label
        m_labelName = new QLabel(name, this);
        m_labelName->setStyleSheet("font-weight: bold; font-size: 14px;"); // 设置名称标签样式

        // 3) 未读气泡 Label（红色圆点 + 白字）
        m_badge = new QLabel(this);
        m_badge->setFixedSize(16, 16); // 设置固定大小
        m_badge->setAlignment(Qt::AlignCenter); // 文本居中对齐
        m_badge->setStyleSheet( // 设置样式表
            "background: red; "
            "color: white; "
            "font-size: 10px; "
            "border-radius: 8px;" // 圆角
            );
        m_badge->setVisible(false); // 初始隐藏未读消息徽章

        // 布局：水平排列 [头像][名称][弹簧][未读气泡]
        QHBoxLayout *h = new QHBoxLayout(this);
        h->setContentsMargins(8, 4, 8, 4); // 设置边距
        h->setSpacing(8); // 设置控件间距
        h->addWidget(m_icon); // 添加头像
        h->addWidget(m_labelName); // 添加名称标签
        h->addStretch(1); // 添加弹性空间，将未读徽章推到右侧
        h->addWidget(m_badge); // 添加未读消息徽章
        setLayout(h); // 应用布局

        // 固定高度
        setFixedHeight(48);
    }

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

    QString m_name;        // 好友名称
    int m_unreadCount;     // 未读消息数量
    QLabel *m_icon;        // 头像标签
    QLabel *m_labelName;   // 名称标签
    QLabel *m_badge;       // 未读消息徽章标签
};