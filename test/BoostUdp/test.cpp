#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <thread>
#include <chrono>

using boost::asio::ip::udp;
using boost::asio::ip::address;

const short broadcast_port = 30001;
const std::string broadcast_address_str = "192.168.216.255"; // 或者一个特定的子网广播地址，例如 "192.168.1.255"

class UDPServer {
public:
    UDPServer(boost::asio::io_context& io_context)
        : socket_(io_context) {
        // 绑定到任意地址的指定端口，用于接收
        udp::endpoint endpoint(boost::asio::ip::address::from_string("192.168.216.76"), broadcast_port);
        socket_.open(endpoint.protocol());
        socket_.set_option(boost::asio::socket_base::reuse_address(true));
        socket_.bind(endpoint);
        std::cout << "UDP Server listening on port " << broadcast_port << std::endl;
        start_receive();
    }

    void start_receive() {
        socket_.async_receive_from(
            boost::asio::buffer(recv_buffer_),
            remote_endpoint_,
            boost::bind(&UDPServer::handle_receive,
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }

private:
    void handle_receive(const boost::system::error_code& error,
                        std::size_t bytes_transferred) {
        if (!error) {
            
            std::string message(recv_buffer_.data(), bytes_transferred);
            if(remote_endpoint_.address() == socket_.local_endpoint().address()){
                std::cout << "本地回环" << std::endl;
            }
            else {
                std::cout << "Received broadcast from " << remote_endpoint_.address().to_string()
                      << ":" << remote_endpoint_.port()
                      << " - Message: " << message << std::endl;
            }

            // 如果需要回复，可以在这里添加发送逻辑
            // std::string reply_message = "Message received by server";
            // socket_.send_to(boost::asio::buffer(reply_message), remote_endpoint_);

            start_receive(); // 继续接收下一个消息
        } else {
            std::cerr << "Receive error: " << error.message() << std::endl;
        }
    }

    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    std::array<char, 1024> recv_buffer_;
};

void send_broadcast(boost::asio::io_context& io_context, const std::string& message) {
    try {
        udp::socket socket(io_context, udp::endpoint(udp::v4(), 0)); // 0表示由系统选择一个可用端口
        socket.set_option(boost::asio::socket_base::broadcast(true));

        udp::endpoint broadcast_endpoint(
            boost::asio::ip::address::from_string(broadcast_address_str),
            broadcast_port);

        socket.send_to(boost::asio::buffer(message), broadcast_endpoint);
        //std::cout << "Sent broadcast message: " << message << " to " << broadcast_address_str << ":" << broadcast_port << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Broadcast send exception: " << e.what() << std::endl;
    }
}

int main() {
    try {
        boost::asio::io_context io_context;

        // 启动接收服务器
        UDPServer server(io_context);

        // 创建一个线程来运行io_context，以便服务器可以异步接收消息
        std::thread t([&io_context](){
            try {
                io_context.run();
            } catch (std::exception& e) {
                std::cerr << "io_context exception: " << e.what() << std::endl;
            }
        });

        // 主线程定期发送广播消息
        int count = 0;
        while (count < 10) { // 发送5次广播作为示例
            std::this_thread::sleep_for(std::chrono::seconds(2));
            std::string message = "Hello from Boost.Asio broadcaster! Count: " + std::to_string(count++);
            send_broadcast(io_context, message);
        }

        // 等待一段时间让服务器接收消息 (或者使用其他同步机制)
        std::this_thread::sleep_for(std::chrono::seconds(5));

        io_context.stop(); // 请求io_context停止
        if (t.joinable()) {
            t.join(); // 等待io_context线程结束
        }

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
