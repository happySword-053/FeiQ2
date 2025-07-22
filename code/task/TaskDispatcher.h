#pragma once
//任务分发器 （中控）

#include"../networkModule/NetworkManager.h"
#include"../fileSystem/FileSystem.h"
#include<QObject>
#include"../dataProcess/DataProcess.h"
#include"../infoModule/chatMessage/chatMessageDAO.h"
#include<QString>
#include"../MyControl/MyWidget/MessageBubble.h"
#include"../infoModule/FriendInfo.h"
#include<list>
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
    /*
    中控解压完数据后，可用以下公共接口实现相关功能
    */

    //获取好友信息
    FriendInfo getFriendInfo(const QString& friendMac); 

    /*----网络相关------*/
    //发送数据包



    /*----文件相关------*/
    
private:
    //获取好友历史聊天记录(首次获取历史记录)
    std::list<MessageBubble*> getFriendHistory(const QString& friendMac);
    //获取好友历史聊天记录(获取更多历史记录)
    std::list<MessageBubble*> getFriendHistoryByTime(const QString& friendMac, const QString& time);
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
    void onNetworkMessageReceived(std::vector<char>&& data);

    /*---------前端给的信号连接的槽函数-----------*/
    // 本地文件管理槽 ////////////

    // 添加文件
    void addFile(const QString& fileName);
    // 删除文件
    void deleteFile(const QString& fileName);

    // 聊天消息槽 ////////////
    // 发送消息
    void sendMessage(const QString& message, const QString& friendIP);

    // 文件传输槽////////////
    // 请求接收某个文件
    void requestFile(const QString& fileName, const QString& friendIP);

    // 网络槽 /////////////
    // 发送广播
    void udp_broadcast_slot();  
};
