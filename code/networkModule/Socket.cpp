#include "Socket.h"

/*-------TcpModule类-----------*/
void TcpModule::handle_accept(const boost::system::error_code &error, boost::asio::ip::tcp::socket new_socket)
{
    if(error){
        LOG(std::string("accept失败: ") + error.message(), ERROR);
        throw std::runtime_error("accept失败"); // 可选：向上传递异常或处理
    }
    
}

Session TcpModule::createSession(std::string ip, uint32_t port)
{
    try{
        //创建一个套接字
        boost::asio::ip::tcp::socket socket(io_context_);
        //连接到目标地址
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip), port);
        socket.connect(endpoint);
        //获取用户信息
        auto userInfo = UserInfoHelper::getInstance().getUserInfo();
        //获取当前的适配器信息
        AdapterInfo adapterInfo;
        Adapter my_adapter = adapterInfo.getNowAdapter();
        //创建一个Session对象
        Session session(std::move(socket));
        session.start();

        {
            //加锁
            std::unique_lock<std::mutex> lock(mtx);
            this->sessions_.emplace_back(std::move(session));
        }
        
        LOG("创建Session成功,目标ip：" + ip + "\n", INFO);

    }catch (const std::exception& e) {
        LOG(std::string("创建Session失败: ") + e.what(), ERROR);
        throw std::runtime_error("创建Session失败"); // 可选：向上传递异常或处理
    }
}

TcpModule::TcpModule()
    : socket_(io_context_),
      work_guard_(boost::asio::make_work_guard(io_context_)) // 直接构造 work_guard

{
    try {
        //获取一个当前已连接的适配器
        AdapterInfo adapterInfo;
        auto adp = adapterInfo.getNowAdapterOnline();
        if (adp.ipv4.empty()) {
            throw std::runtime_error("未获取到有效适配器 IP");
        }

        // 提取 IP 地址
        std::string ip;
        auto index = adp.ipv4.find("/");
        if(index != std::string::npos){
            ip = adp.ipv4.substr(0, index);
        } else {
            ip = adp.ipv4;  // 兼容无分隔符的情况（如直接存储 IP）
        }
        if (ip.empty()) {
            throw std::runtime_error("适配器 IP 地址为空");
        }
        this->ip_ = ip;
        this->port_ = 9000;
        // 初始化 acceptor（需确保端口未被占用）
        this->acceptor_ = std::make_unique<boost::asio::ip::tcp::acceptor>(
            io_context_, 
            boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ip_), this->port_)
        );
        LOG("TcpModule构造成功", INFO);
    } catch (const std::exception& e) {
        LOG(std::string("TcpModule构造失败: ") + e.what(), ERROR);
        throw std::runtime_error("tcp模块构建时报错"); // 可选：向上传递异常或处理
    }
  }

  int TcpModule::start()
  {
      try {
        this->acceptor_->async_accept(
            std::bind(
                &TcpModule::handle_accept,
                shared_from_this(),  // 绑定当前对象的 shared_ptr
                std::placeholders::_1,  // 错误码
                std::placeholders::_2   // 新连接的 socket
            )
        );
      }catch (const std::exception& e) {
        LOG(std::string("TcpModule启动失败: ") + e.what(), ERROR);
        throw std::runtime_error("tcp模块启动时报错"); // 可选：向上传递异常或处理
        
      }
  }
/*-------Session类-----------*/
  void Session::doread()
  {
      auto self = shared_from_this();  // 获取当前对象的 shared_ptr
      this->socket_.async_read_some(
          boost::asio::buffer(this->recv_buffer_, 1024 * 9),  // 读取数据到缓冲区
          [this, self](const boost::system::error_code &error, std::size_t bytes_transferred) {  // 异步读取完成后的回调
              if(error){
                  LOG(std::string("read失败: ") + error.message(), ERROR);
                  throw std::runtime_error("read失败,关闭本套接字"); // 可选：向上传递异常或处理
              }
            }
      );
  }

  void Session::stop()
  {
    //先获取两个锁，不允许读写操作
    std::unique_lock<std::mutex> lock1(this->recv_mtx, std::defer_lock);
    std::unique_lock<std::mutex> lock2(this->send_mtx, std::defer_lock);
    std::lock(lock1, lock2);  // 同时获取两个锁
    //关闭socket
    this->socket_.close();
    LOG("已断开连接,关闭其套接字", INFO);
  }
