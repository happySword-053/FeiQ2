#include "TaskDispatcher.h"

TaskDispatcher::TaskDispatcher(QObject *parent)
{
    //构建网络模块
    this->networkManager = std::make_shared<NetworkManager>();  // 创建 NetworkManager 对象的智能指针并初始化
    //构建文件系统模块
    this->fileSystem = std::make_shared<FileSystem>();  // 创建 FileSystem 对象的智能指针并初始化
}