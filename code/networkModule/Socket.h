#pragma once
#include<boost/asio.hpp>
#include<thread>
#include<vector>
#include<mutex>  // 新增：互斥锁头文件
#include<memory>  // 新增：智能指针头文件
#include"../logs/logs.h"
#include"../systemInfoHelper/AdapterInfo.h"
#include"../systemInfoHelper/userLocalInfo.h"

#define UDP_PORT 10000
#define TCP_RECV_PORT 10001
#define TCP_SEND_PORT 10002

//表示已连接的会话
class Session: public std::enable_shared_from_this<Session>{
private:
    boost::asio::ip::tcp::socket socket_;  // 套接字
    Adapter adapterInfo;
    UserInfo userInfo;
    std::vector<char> recv_buffer_;  // 接收缓冲区
    std::vector<char> send_buffer_;  // 发送缓冲区
    std::mutex recv_mtx;  // 用于同步读写操作的互斥锁
    std::mutex send_mtx;  // 用于同步读写操作的互斥锁
    
public:
    Session(boost::asio::ip::tcp::socket &&socket) : socket_(std::move(socket)) {}  // 构造函数
    //移动构造函数
    Session(Session&& other) noexcept : socket_(std::move(other.socket_)) {
        // 移动其他对象的成员变量到当前对象
        recv_buffer_ = std::move(other.recv_buffer_);
        send_buffer_ = std::move(other.send_buffer_);
        // 重置其他对象的成员变量
        other.recv_buffer_.clear();
        other.send_buffer_.clear();
        other.socket_ = boost::asio::ip::tcp::socket(other.socket_.get_executor());
        
        userInfo = other.userInfo;
        adapterInfo = other.adapterInfo;
        other.stop();
    }
    ~Session() {    
    }
    void start(){
        doread();  // 启动读取操作
    } // 启动会话
    void doread();//读取数据
    void dowrite();//写入数据
    int handel_data();//处理数据
    void stop();//停止会话
    
};
//封装一个tcp会话类
class TcpModule : public std::enable_shared_from_this<TcpModule> {
private:
    /*----------成员变量-----*/
    //ip和端口
    std::string ip_;
    unsigned short port_;
    boost::asio::ip::tcp::socket socket_;
    std::mutex mtx;
    boost::asio::io_context io_context_;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_; 
    std::vector<Session> sessions_;
    //std::thread io_thread_;  // 新增：运行 io_context 的线程
    /*----------工作任务-----*/
    void handle_accept(const boost::system::error_code& error,boost::asio::ip::tcp::socket new_socket);  // 新增：处理连接的函数
    
    Session createSession(std::string ip, uint32_t port);  // 新增：创建会话的函数,注意已包含连接工作

public:
    // 在初始化列表中完成所有成员初始化
    TcpModule();

    ~TcpModule() {
         // 确保 work_guard 被正确释放
        work_guard_.reset();
        // 确保 io_context 停止运行
        io_context_.stop();
        //关闭socket
        socket_.close();
        
    }
    int start();//启动函数
    int stop();//停止函数
};

//封装一个udp会话类
class UdpModule : public std::enable_shared_from_this<UdpModule> {
private:
    boost::asio::io_context io_context_; // UdpModule 内部管理的 io_context
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint sender_endpoint_; // 用于接收时存储发送方信息
    boost::asio::ip::udp::endpoint broadcast_endpoint_; // 用于发送广播的目标
    std::vector<char> recv_buffer_; // 仅保留接收缓冲区
    std::mutex mtx_;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_; // 确保 io_context 持续运行
    std::thread io_thread_; // 运行 io_context 的线程

    void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handle_send(const boost::system::error_code& error, std::size_t bytes_transferred, std::shared_ptr<std::string> message_ptr);

public:
    UdpModule(unsigned short listen_port, unsigned short broadcast_port);
    ~UdpModule();

    void start_receive();
    void send_broadcast(const std::string& message);

    std::function<void(const std::string&, const boost::asio::ip::udp::endpoint&)> on_message_received_;
    
    void stop(); // 添加停止方法
};