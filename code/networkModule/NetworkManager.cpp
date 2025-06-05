#include "NetworkManager.h"

NetworkManager::NetworkManager(QObject *parent = nullptr): QObject(parent)
{
    this->tcpModule = std::make_shared<TcpModule>();  // 创建 TcpModule 对象的智能指针并初始化
    this->udpModule = std::make_shared<UdpModule>(*tcpModule);  // 创建 UdpModule 对象的智能指针并初始化
    //创建一个function
    std::function<void(std::vector<char>&&)> on_data_control = bind(&NetworkManager::on_data_control_tcp, this, std::placeholders::_1);  // 创建回调函数
    this->tcpModule->set_on_data_control(on_data_control);  // 设置回调函数
    std::function<void(std::vector<char>&&)> on_data_control_udp = bind(&NetworkManager::on_data_control_udp, this, std::placeholders::_1);  // 创建回调函数
    this->udpModule->set_on_data_control(on_data_control_udp);  // 设置回调函数
}

void NetworkManager::on_data_control_tcp(std::vector<char> &&data)
{
    auto tem_data = std::move(data);  // 移动数据到局部变量中
    emit this->recvMessageSignal(std::move(tem_data));  // 发射信号并移动数据
}

void NetworkManager::on_data_control_udp(std::vector<char> &&data)
{
    auto tem_data = std::move(data);  // 移动数据到局部变量中
    emit this->recvMessageSignal(std::move(tem_data));  // 发射信号并移动数据
}
void NetworkManager::sendMessageSlot(const std::vector<char>& data){
    
}