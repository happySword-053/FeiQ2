#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QApplication>
#include <QMainWindow>
#include <QSplitter>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QDateTime>
#include <QScrollBar>
#include <QListWidgetItem>
#include <QMainWindow>
#include"FriendItemWidget.h"
#include"sharefilemanagerwidget.h"
#include"settingswidget.h"
// QT_BEGIN_NAMESPACE
// namespace Ui {
// class MainWindow;
// }
// QT_END_NAMESPACE
// 用来自定义“好友列表”中每一行的 Widget

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr); // 构造函数
    ~MainWindow();                         // 析构函数

private:
    //Ui::MainWindow *ui; // UI界面指针
    std::function<void()> friendsInterface;// 好友列表相关接口  回调函数

    private slots:
    // 当切换好友时，清除该好友未读气泡
    void onFriendSelectionChanged(QListWidgetItem *current, QListWidgetItem *previous) {
        Q_UNUSED(previous);
        if (!current) return;
        auto *fw = static_cast<FriendItemWidget*>(m_friendList->itemWidget(current));
        if (fw) {
            fw->clearUnread();
        }
        // （此处未实现切换好友后加载实际聊天记录，只示范清空未读气泡）
    }

    // 点击“发送”
    void onSendClicked() {
        QString text = m_inputEdit->toPlainText().trimmed();
        if (text.isEmpty()) return;

        // 当前好友名称
        QListWidgetItem *currentItem = m_friendList->currentItem();
        QString friendName;
        if (currentItem) {
            auto *fiw = static_cast<FriendItemWidget*>(m_friendList->itemWidget(currentItem));
            friendName = fiw->getName();
        } else {
            friendName = "Unknown";
        }

        insertOutgoingMessage(friendName, text);
        m_inputEdit->clear();

        // 滚动到底部
        QScrollBar *bar = m_scrollArea->verticalScrollBar();
        bar->setValue(bar->maximum());
    }

    // 点击“模拟接收”
    void onSimulateReceive() {
        const QString simulatedSender = "Alice";
        const QString simulatedText   = "这是来自 Alice 的新消息！";

        // 找到 Alice 对应的列表项
        for (int i = 0; i < m_friendList->count(); ++i) {
            QListWidgetItem *item = m_friendList->item(i);
            auto *fiw = static_cast<FriendItemWidget*>(m_friendList->itemWidget(item));
            if (fiw && fiw->getName() == simulatedSender) {
                if (item == m_friendList->currentItem()) {
                    // 如果 Alice 正在聊天，就直接显示消息
                    insertIncomingMessage(simulatedSender, simulatedText);
                    QScrollBar *bar = m_scrollArea->verticalScrollBar();
                    bar->setValue(bar->maximum());
                } else {
                    // 如果没在聊天，增加未读气泡
                    fiw->incrementUnread();
                }
                break;
            }
        }
    }

private:
    // 插入“对方”消息（左对齐，黄色气泡）
    void insertIncomingMessage(const QString &sender, const QString &content) {
        // 1) 头部 [HH:mm:ss] Sender
        QString timeStr = QDateTime::currentDateTime().toString("HH:mm:ss");
        QLabel *header = new QLabel(QString("[%1] %2").arg(timeStr).arg(sender));
        header->setStyleSheet("color: #555; font-size:12px;");

        QWidget *headerContainer = new QWidget;
        QHBoxLayout *headerHL = new QHBoxLayout(headerContainer);
        headerHL->setContentsMargins(0, 0, 0, 0);
        headerHL->setSpacing(0);
        headerHL->addWidget(header);
        headerHL->addStretch(1);  // 左对齐

        // 2) 气泡正文（黄色背景）
        QLabel *bubble = new QLabel(content);
        bubble->setWordWrap(true);
        bubble->setStyleSheet(
            "background: #fff1a8; "
            "padding:6px; "
            "border-radius:4px; "
            "font-size:14px;"
        );
        QWidget *bubbleContainer = new QWidget;
        QHBoxLayout *bubbleHL = new QHBoxLayout(bubbleContainer);
        bubbleHL->setContentsMargins(0, 0, 0, 0);
        bubbleHL->setSpacing(0);
        bubbleHL->addWidget(bubble);
        bubbleHL->addStretch(1);  // 左对齐

        m_chatLayout->addWidget(headerContainer);
        m_chatLayout->addWidget(bubbleContainer);
    }

    // 插入“我”消息（右对齐，蓝色气泡）
    void insertOutgoingMessage(const QString &friendName, const QString &content) {
        // 1) 头部 [HH:mm:ss] Me → friendName
        QString timeStr = QDateTime::currentDateTime().toString("HH:mm:ss");
        QLabel *header = new QLabel(QString("[%1] %2").arg(timeStr).arg("Me → " + friendName));
        header->setStyleSheet("color: #555; font-size:12px;");

        QWidget *headerContainer = new QWidget;
        QHBoxLayout *headerHL = new QHBoxLayout(headerContainer);
        headerHL->setContentsMargins(0, 0, 0, 0);
        headerHL->setSpacing(0);
        headerHL->addStretch(1);    // 右对齐
        headerHL->addWidget(header);

        // 2) 气泡正文（蓝色背景）
        QLabel *bubble = new QLabel(content);
        bubble->setWordWrap(true);
        bubble->setStyleSheet(
            "background:#d0eaff; "
            "padding:6px; "
            "border-radius:4px; "
            "font-size:14px;"
        );
        QWidget *bubbleContainer = new QWidget;
        QHBoxLayout *bubbleHL = new QHBoxLayout(bubbleContainer);
        bubbleHL->setContentsMargins(0, 0, 0, 0);
        bubbleHL->setSpacing(0);
        bubbleHL->addStretch(1);      // 右对齐
        bubbleHL->addWidget(bubble);

        m_chatLayout->addWidget(headerContainer);
        m_chatLayout->addWidget(bubbleContainer);
    }

private:
    QListWidget   *m_friendList;
    QWidget       *m_chatArea;
    QVBoxLayout   *m_chatLayout;
    QScrollArea   *m_scrollArea;
    QTextEdit     *m_inputEdit;
    QPushButton   *m_sendBtn;
    QPushButton   *m_simulateBtn;
    // 分享文件管理窗口
    ShareFileManagerWidget *shareFileManagerWidget;
    // 设置窗口
    SettingsWidget *settingsWidget;
    
};
#endif // MAINWINDOW_H