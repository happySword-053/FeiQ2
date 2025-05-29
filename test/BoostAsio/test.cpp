#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <mutex>  // 新增：互斥锁头文件

using boost::asio::ip::tcp;

// 会话：负责一个连接的读写
class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, std::vector<std::shared_ptr<Session>>& sessions, std::mutex& sessions_mutex)
      : socket_(std::move(socket)), 
        sessions_(sessions), 
        sessions_mutex_(sessions_mutex)
    {}

    // 启动会话
    void start() {
        do_read();
    }

    void do_write(const std::string& msg) {
        auto self = shared_from_this();
        boost::asio::async_write(
            socket_,
            boost::asio::buffer(msg),
            [this, self](boost::system::error_code ec, std::size_t /*sent*/) {
                if (ec) {
                    std::cout << "[" << std::this_thread::get_id() 
                              << "] Write error: " << ec.message() << "\n";
                    // 断开时从sessions中移除当前会话（需加锁）
                    std::lock_guard<std::mutex> lock(sessions_mutex_);
                    sessions_.erase(std::remove(sessions_.begin(), sessions_.end(), self), sessions_.end());
                    socket_.close();
                }                
                else {
                    std::cout << "[" << std::this_thread::get_id() 
                              << "] Write success\n"; 
                }
            }
        );
    }

private:
    void do_read() {
        auto self = shared_from_this();
        socket_.async_read_some(
            boost::asio::buffer(recv_buffer), 
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::string msg(recv_buffer.data(), length);
                    std::cout << "[" << std::this_thread::get_id() 
                              << "] Received: " << msg << "\n";
                    recv_buffer.fill(0);  // 清空缓冲区
                    do_read();  // 继续读取
                }
                else {
                    std::cout << "[" << std::this_thread::get_id() 
                              << "] Client disconnected\n";
                    // 断开时从sessions中移除当前会话（需加锁）
                    std::lock_guard<std::mutex> lock(sessions_mutex_);
                    sessions_.erase(std::remove(sessions_.begin(), sessions_.end(), self), sessions_.end());
                    socket_.close();
                }
            }
        );
    }

    tcp::socket socket_;
    std::array<char, 1024> recv_buffer;
    // 新增：引用外部会话容器和互斥锁
    std::vector<std::shared_ptr<Session>>& sessions_;
    std::mutex& sessions_mutex_;
};

// 写进程（需加锁保护sessions）
void write_process(std::vector<std::shared_ptr<Session>>& sessions, std::mutex& sessions_mutex) {
    while (true) {
        std::string msg;
        std::getline(std::cin, msg);  // 从标准输入读取消息
        if (msg.empty()) continue;  // 跳过空消息

        // 加锁遍历会话（避免遍历时sessions被修改）
        std::lock_guard<std::mutex> lock(sessions_mutex);
        for (auto& session : sessions) {
            session->do_write(msg);  // 异步写入消息
        }
    } 
}

int main() {
    try {
        const unsigned short port = 12345;
        const std::size_t thread_count = 3;

        boost::asio::io_context io_context;
        auto work_guard = boost::asio::make_work_guard(io_context);

        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

        // 新增：会话容器和互斥锁
        std::vector<std::shared_ptr<Session>> sessions;
        std::mutex sessions_mutex;

        // 异步accept循环
        std::function<void()> do_accept;
        do_accept = [&]() {
            acceptor.async_accept(
                [&](boost::system::error_code ec, tcp::socket socket) {
                    if (!ec) {
                        std::cout << "[" << std::this_thread::get_id() 
                                  << "] New connection\n";
                        // 创建Session时传入sessions和互斥锁，并加入容器（需加锁）
                        auto session = std::make_shared<Session>(
                            std::move(socket), sessions, sessions_mutex
                        );
                        {
                            std::lock_guard<std::mutex> lock(sessions_mutex);
                            sessions.push_back(session);
                        }
                        session->start();
                    }
                    do_accept();  // 持续接收连接
                }
            );
        };
        do_accept();

        // 启动线程池（处理网络IO）
        std::vector<std::thread> threads;
        for (std::size_t i = 0; i < thread_count; ++i) {
            threads.emplace_back([&io_context](){
                io_context.run();
            });
        }

        // 新增：启动写线程（处理标准输入）
        std::thread write_thread(write_process, std::ref(sessions), std::ref(sessions_mutex));

        std::cout << "Server running on port " << port 
                  << " with " << thread_count << " threads.\n";

        // 等待线程退出（需等待写线程）
        write_thread.join();
        for (auto& t : threads) {
            t.join();
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}