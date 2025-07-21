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
#include<QMap>
#include"../MyControl/MyWidget/FriendChatWidget.h"
#include"../MyControl/MyWidget/FriendItemWidget.h"
#include"../MyControl/MyWidget/MessageBubble.h"
#include"sharefilemanagerwidget.h"
#include"settingswidget.h"
// QT_BEGIN_NAMESPACE
// namespace Ui {
// class MainWindow;
// }
// QT_END_NAMESPACE
// 注释掉的UI命名空间，可能是Qt Designer生成的代码

class MainWindow : public QMainWindow  // 定义MainWindow类，继承自QMainWindow
{
    Q_OBJECT  // Qt元对象宏，支持信号槽机制

public:
    MainWindow(QWidget *parent = nullptr); // 构造函数声明，parent为父窗口指针
    ~MainWindow();                         // 析构函数声明

private slots:
    void onScrollBarValueChanged(int value); // 滚动条值变化的槽函数声明

private:
    // Ui::MainWindow *ui; // 注释掉的UI指针，可能是Qt Designer生成的
    

private slots:
    // 当切换好友时，清除该好友未读气泡
    void onFriendSelectionChanged(QListWidgetItem *current, QListWidgetItem *previous);

    // 点击“发送”按钮的槽函数
    void onSendClicked();

    // 点击“模拟接收”按钮的槽函数
    void onSimulateReceive();

private:
    // 插入“对方”消息（左对齐，黄色气泡）
    void insertIncomingMessage(const QString &sender, const QString &content) ;

    // 插入“我”发送的消息（右对齐，蓝色气泡）
    void insertOutgoingMessage(const QString &friendName, const QString &content);

    // 向中控模块发送获取好友历史聊天记录(首次获取历史记录)
    std::function<void()> getFriendHistory;

    // 向中控模块发送获取好友历史聊天记录(获取更多历史记录)
    std::function<void()> getFriendHistoryByTime;

    // 向中控模块发送获取好友列表
    std::function<void()> getFriendsList; 
public:
    // 设置好友列表相关接口的回调函数
    void setFriendsInterface(std::function<void()> friendsInterface){
        getFriendsList = friendsInterface;
    }
    // 设置获取好友历史聊天记录的回调函数
    void setGetFriendHistory(std::function<void()> getFriendHistory){
        this->getFriendHistory = getFriendHistory;
    }
    // 设置获取更多好友历史聊天记录的回调函数
    void setGetFriendHistoryByTime(std::function<void()> getFriendHistoryByTime){
        this->getFriendHistoryByTime = getFriendHistoryByTime;
    }
private:
    QListWidget   *m_friendList; // 好友列表控件指针
    QWidget       *m_chatArea;   // 聊天区域控件指针
    QVBoxLayout   *m_chatLayout; // 聊天区域布局指针
    QScrollArea   *m_scrollArea; // 滚动区域控件指针
    QTextEdit     *m_inputEdit;  // 输入框控件指针
    QPushButton   *m_sendBtn;    // 发送按钮指针
    QPushButton   *m_simulateBtn;// 模拟接收按钮指针
    ShareFileManagerWidget *shareFileManagerWidget; // 文件共享管理窗口指针
    SettingsWidget *settingsWidget; // 设置窗口指针
    QMap<QListWidgetItem *, QWidget *> m_friendWidgets; // 好友控件映射表
};

#endif // MAINWINDOW_H  // 头文件结束宏