#include "NetworkManager.h"

NetworkManager::NetworkManager(QObject *parent): QObject(parent)
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
    
    //解密数据
    try{
        tem_data = this->dataProcess.decrypt(tem_data);  // 解密数据
    }catch(const std::exception& e){
        LOG(std::string("on_data_control_tcp槽函数收到错误") + e.what(), ERROR);  // 记录错误信息
        throw std::runtime_error("on_data_control_tcp槽函数收到错误");  // 抛出异常，用于上层处理
    }
    emit this->recvMessageSignal(std::move(tem_data));  // 发射信号并移动数据
}

void NetworkManager::on_data_control_udp(std::vector<char> &&data)
{
    auto tem_data = std::move(data);  // 移动数据到局部变量中
    
    //处理数据
    try{
        tem_data = this->dataProcess.decrypt(tem_data);  // 解密数据
        //解压缩数据
        tem_data = this->dataProcess.decompress_vector(tem_data);  // 解压缩数据
    }catch(const std::exception& e){
        LOG(std::string("on_data_control_tcp槽函数收到错误") + e.what(), ERROR);  // 记录错误信息
        throw std::runtime_error("on_data_control_tcp槽函数收到错误");  // 抛出异常，用于上层处理
    }
    emit this->recvMessageSignal(std::move(tem_data));  // 发射信号并移动数据
}

void NetworkManager::tcp_sendMessageSlot(const std::string& ip, std::vector<char>& data){
    try{

        data = this->dataProcess.compress_vector(data);  // 压缩数据
        data = this->dataProcess.encrypt(data);  // 加密数据
        this->tcpModule->sendData(ip,move(data));  // 调用 TcpModule 的 sendData 方法发送数据
    }catch(const std::exception& e){
        LOG(std::string("tcp_sendMessageSlot槽函数收到错误") + e.what(), ERROR);  // 记录错误信息
        throw std::runtime_error("tcp_sendMessageSlot槽函数收到错误");  // 抛出异常，用于上层处理
    }
}

void NetworkManager::rebind(const std::string &ip)
{
    try{
       this->udpModule->rebind(ip);  // 调用 UdpModule 的 rebind 方法重新绑定 
    }catch(const std::exception& e){
        LOG(std::string("rebind槽函数收到错误") + e.what(), ERROR);  // 记录错误信息
        throw std::runtime_error("rebind槽函数收到错误");  // 抛出异常，用于上层处理
    }
}

void NetworkManager::udp_broadcast_slot() {
    try{
        this->udpModule->broadcast();  // 调用 UdpModule 的 broadcast 方法进行广播
    }catch(const std::exception& e){
        LOG(std::string("udp_broadcast_slot槽函数收到错误") + e.what(), ERROR);  // 记录错误信息

        throw std::runtime_error("udp_broadcast_slot槽函数收到错误");  // 抛出异常，用于上层处理
    }
}