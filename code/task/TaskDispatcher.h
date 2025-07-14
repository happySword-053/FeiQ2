#pragma once
//任务分发器 （中控）
/*
1、解密数据同时解析数据类型，进一步剖析数据然后上传给高层
2、根据信号类型，压缩数据同时加密数据，然后上传给底层
*/
#include"../networkModule/NetworkManager.h"
#include"../fileSystem/FileSystem.h"
#include<QObject>
#include"../dataProcess/DataProcess.h"
#include"../infoModule/chatMessage/chatMessageDAO.h"
#include<QString>
#include"../MyControl/MyWidget/MessageBubble.h"
#include<list>
class TaskDispatcher :public QObject{
    Q_OBJECT
public:
    TaskDispatcher(QObject* parent = nullptr);
    //~TaskDispatcher();
    //获取好友历史聊天记录(首次获取历史记录)
    std::list<MessageBubble*> getFriendHistory(const QString& friendMac);
    //获取好友历史聊天记录(获取更多历史记录)
    std::list<MessageBubble*> getFriendHistoryByTime(const QString& friendMac, const QString& time);
private:
    NetworkManager networkManager;  // 网络功能模块
    FileSystem fileSystem;  // 文件系统模块
    DataProcess dataProcess;  // 数据处理模块

    /*DAO层*/
    ChatMessageDAO chatMessageDAO;
};
