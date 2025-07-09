#pragma once
//任务分发器 
/*
1、解密数据同时解析数据类型，进一步剖析数据然后上传给高层
2、根据信号类型，压缩数据同时加密数据，然后上传给底层
*/
#include"../networkModule/NetworkManager.h"
#include"../fileSystem/FileSystem.h"
#include<QObject>

class TaskDispatcher :public QObject{
    Q_OBJECT
public:
    TaskDispatcher(QObject* parent = nullptr);
    //~TaskDispatcher();
    
private:
    std::shared_ptr<NetworkManager> networkManager;  // 网络功能模块
    std::shared_ptr<FileSystem> fileSystem;  // 文件系统模块
};
