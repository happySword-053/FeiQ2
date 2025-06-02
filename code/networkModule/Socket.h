#pragma once
#include<boost/asio.hpp>
#include<thread>
#include<vector>
#include<mutex>  // 新增：互斥锁头文件
#include<memory>  // 新增：智能指针头文件
#include<unordered_set>
#include"../logs/logs.h"
#include<array>
#include"../systemInfoHelper/APIHelper.h"
#include"NetworkHelper.h"
#include"../task/CurrentTask.h"
#include"../threadPool/ThreadPool.h"

#define UDP_PORT 30000
#define TCP_PORT 30001


//表示已连接的会话
class Session: public std::enable_shared_from_this<Session>{
private:
    boost::asio::ip::tcp::socket socket_;  // 套接字
    // UserInfoAndNetworkInfo& userInfoAndNetworkInfo;//用户信息和网络信息
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
        // other.stop();
    }
    ~Session() {    
    }
    void start(){
        doread();  // 启动读取操作
    } // 启动会话
    void doread();//读取数据
    void dowrite();//写入数据
    int handle_data();//处理数据
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
    std::vector<std::shared_ptr<Session>> sessions_;  // 改为 shared_ptr<Session>
    std::array<std::thread, 2> io_thread_;  // 新增：运行 io_context 的线程
    /*----------工作任务-----*/
    void handle_accept(const boost::system::error_code& error,boost::asio::ip::tcp::socket new_socket);  // 新增：处理连接的函数
    
    void createSession(std::string ip);  // 改为 void 类型

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
    void init();//初始化函数
    int start();//启动函数
    int stop();//停止函数 
    void rebind(const std::string& ip);//重新绑定socket到ip
    void connect_by_udpEndpoint(std::unordered_set<boost::asio::ip::udp::endpoint> endpoints);
};

//封装一个udp会话类
class UdpModule : public std::enable_shared_from_this<UdpModule> {
private:
    /*----------成员变量-----*/
    boost::asio::io_context io_context_;
    boost::asio::ip::udp::socket socket_;
    std::unordered_set<boost::asio::ip::udp::endpoint> endpoints_;//用来在多次广播后检测是否有重复的ip
    std::mutex endpoints,buffer_mtx;
    std::vector<char> recv_buffer_;  // 接收缓冲区
    boost::asio::ip::udp::endpoint remote_endpoint_;  // 远程端点
    std::array<std::thread, 2> io_thread_;  // 新增：运行 io_context 的线程
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_;
    //UserInfoAndNetworkInfo& userInfoAndNetworkInfo;//用户信息和网络信息
    TcpModule& tcpModule;
    /*----------工作任务-----*/

    void handel_receive(boost::asio::ip::udp::endpoint  remote_endpoint);//接收任务
    
public:
    UdpModule(TcpModule& tcpModule)
    : io_context_(),
    tcpModule(tcpModule),//初始化tcpModule
    socket_(io_context_),
    work_guard_(boost::asio::make_work_guard(io_context_)) // 直接构造 work_guard
    {
        init();
        for (auto& thread : io_thread_) {
            thread = std::thread([this]() {
                io_context_.run();  // 运行 IO 上下文
            });
        }
    }


    ~UdpModule();
    void init();//初始化函数
    void start_recv();//开启读任务
    void broadcast();//广播任务,发送自身的userInfoAndNetworkInfo
    void rebind(const std::string& ip);//重新绑定socket到ip
};