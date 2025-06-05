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


private:
    //给tcp模块的回调函数
    void on_data_control_tcp(std::vector<char>&& data);  // 处理从 TcpModule 接收的数据
    //给udp模块的回调函数
    void on_data_control_udp(std::vector<char>&& data);  // 处理从 UdpModule 接收的数据
    
signals:
    void recvMessageSignal(const std::vector<char>& data);  // 处理从底层模块接收的数据


public slots:
    /*----------槽函数-----*/
    void udp_broadcast_slot(const std::vector<char>& data);  // 处理从上层模块要广播的信号
    void sendMessageSlot(const std::vector<char>& data);  // 处理从上层模块要发送的数据
};