#pragma once
#include"Socket.h"
#include"../logs/logs.h"
#include<QObject>
#include"../MyControl/MetaDataDefine.h"
class NetworkManager :public QObject{
    Q_OBJECT
private:
    /*----------成员变量-----*/
    std::shared_ptr<TcpModule> tcpModule;  // 指向 TcpModule 对象的智能指针
    std::shared_ptr<UdpModule> udpModule;  // 指向 UdpModule 对象的智能指针

public:
    /*----------构造函数-----*/
    explicit NetworkManager(QObject *parent = nullptr);  // 构造函数，初始化 tcpModule 和 udpModule
    ~NetworkManager();  // 析构函数，释放资源
signals:
    void recvMessageSignal(std::vector<char>& data);  // 处理从底层模块接收的数据
    void sendMessageSignal(std::vector<char>& data);  // 处理从底层模块接收的数据
};