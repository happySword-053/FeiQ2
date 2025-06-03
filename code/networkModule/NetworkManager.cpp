#include "NetworkManager.h"

NetworkManager::NetworkManager(QObject *parent = nullptr): QObject(parent)
{
    this->tcpModule = std::make_shared<TcpModule>();  // 创建 TcpModule 对象的智能指针并初始化
    this->udpModule = std::make_shared<UdpModule>(*tcpModule);  // 创建 UdpModule 对象的智能指针并初始化
}
