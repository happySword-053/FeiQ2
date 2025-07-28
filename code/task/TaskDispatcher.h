#pragma once
//任务分发器 （中控）
#include<QObject>
#include<QString>
#include<QMainWindow>

#include<list>

#include"../networkModule/NetworkManager.h"
#include"../fileSystem/FileSystem.h"

#include"../dataProcess/DataProcess.h"
#include"../infoModule/chatMessage/chatMessageDAO.h"

#include"../MyControl/MyWidget/MessageBubble.h"
#include"../infoModule/FriendInfo.h"

#include"../Sqlite3/SqlLite.h"
#include"../MyControl/MetaDataDefine.h"

/*
任务数据应该在本层就应该封装好，传给下层网络层的数据只应该是vector<char>

*/

class TaskDispatcher :public QObject{
    Q_OBJECT
public:
    TaskDispatcher(QObject* parent = nullptr);
    //~TaskDispatcher();

    // 解析数据头将其分发给各个模块
    void parseDataHeader(std::vector<char>& data);
    /*----好友相关------*/
    //获取好友信息
    FriendInfo getFriendInfo(const QString& friendMac); 

    /*----网络相关------*/
    //发送数据包
    void sendPackage(const std::vector<char>&& data);


    /*----文件相关------*/
    
private:
    //获取好友历史聊天记录(首次获取历史记录)
    std::list<MessageBubble*> getFriendHistory(const QString& friendMac);
    //获取好友历史聊天记录(获取更多历史记录)
    std::list<MessageBubble*> getFriendHistoryByTime(const QString& friendMac, const QString& time);
    // 前端指针
    QMainWindow* mainWindow;
private:
    NetworkManager networkManager;  // 网络功能模块
    FileSystem fileSystem;  // 文件系统模块
    
    Friends friends;  // 好友管理模块
    /*DAO层*/
    ChatMessageDAO chatMessageDAO;

signals:
    // 发送消息给网络层（已经加密压缩）
    void sendMessageSignal(const std::vector<char>&& data);
public slots:
    // 收到网络层的消息（需要解密和解压缩），处理完后根据任务类型分发任务到各个模块
    void onNetworkMessageReceived(std::vector<char>&& data){parseDataHeader(data);}

    /*---------前端给的信号连接的槽函数-----------*/
    // 好友管理槽 --------------///////////
    // 查看好友详细
    void getFriendDetail(const FriendInfo& friendInfo);
    // 刷新查看当前在线的好友 （前端点击刷新列表按钮）
    void refreshOnlineFriends();

    // 本地文件管理槽 -------------///////////
    // 添加文件
    void addFile(const QString& fileName);
    // 删除文件
    void deleteFile(const QString& fileName);

    // 聊天消息槽 -------------////////////
    // 前端发送消息
    void sendMessage(const QString& message, const FriendInfo& friendInfo);


    // 文件传输槽-------------////////////
    // 请求接收某个文件
    void requestFile(const QString& fileName, const FriendInfo& friendInfo);
    // 请求文件列表
    void requestFileList(const FriendInfo& friendInfo);

    // 网络槽 ---------------/////////////
    // 发送广播
    void udp_broadcast_slot();  

    // 设置槽 ---------------/////////////
    // 设置自己的用户名
    void setUserName(const QString& userName);
    // 设置头像
    void setAvatar(const QString& avatarPath);
    // 设置个性签名
    void setSignature(const QString& signature);
    // 设置手机电话。。。。。。。。。尽情期待

    //左键单击托盘图标打开主界面
    void trayIconActivated(bool Is);
    // 双击运行程序时最小化
    void doubleClickMinimized(bool Is);
    // 关闭程序时最小化
    void closeToTray(bool Is);
    // 设置保存聊天记录位置
    void setChatHistoryPath(const QString& path);

    // 设置文件接收位置
    void setFileReceivePath(const QString& path);
    // 设置其他用户上线时进行通知
    void setNotifyWhenOnline(bool notify);
    // 设置其他用户下线时进行通知
    void setNotifyWhenOffline(bool notify);
    // 设置自动刷新设置
    void setAutoRefresh(int refreshTime);
    // 设置请勿打扰
    void setDoNotDisturb(bool doNotDisturb);
public slots:
    // 从底层接收数据槽函数
    void receiveDataFromLowerLayer(std::vector<char>&& data);
};
