#include "Socket.h"

/*-------TcpModule类-----------*/
// handle_accept 函数修改
void TcpModule::handle_accept(const boost::system::error_code &error, boost::asio::ip::tcp::socket new_socket) {
    if(error){
        LOG(std::string("accept失败: ") + error.message(), ERROR);
        throw std::runtime_error("accept失败");
    }

    try{
        LOG("接到外来连接",INFO);
        // 使用 shared_ptr 管理 Session
        auto session = std::make_shared<Session>(std::move(new_socket));
        session->set_on_data_control(this->on_data_control_);
        session->start();  // start() 内部通过 shared_from_this() 安全获取指针
        {
            std::unique_lock<std::mutex> lock(mtx);
            this->sessions_.push_back(session);  // 存储 shared_ptr
        }
        LOG("已连接新用户", INFO);
    }catch (const std::exception& e) {
        LOG(std::string("handle_accept失败: ") + e.what(), ERROR);
        throw std::runtime_error("handle_accept失败");
    }
}

// createSession 函数修改
void TcpModule::createSession(std::string ip) {
    try{
        boost::asio::ip::tcp::socket socket(io_context_);
        socket.bind(boost::asio::ip::tcp::endpoint(
            boost::asio::ip::address::from_string(
                NetworkHelper::getInstance().getAdapterInfo().getNowAdapter().ipv4
            ), 0));
        //连接到目标地址
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip), TCP_PORT);
        socket.connect(endpoint);
        //获取用户信息
        auto userInfo = UserInfoHelper::getInstance().getUserInfo();
        //获取当前的适配器信息
        AdapterInfo adapterInfo;
        Adapter my_adapter = adapterInfo.getNowAdapter();
        //创建一个Session对象

        std::shared_ptr<Session> session = std::make_shared<Session>(std::move(socket));
        session->set_on_data_control(this->on_data_control_);
        session->start();
        {
            std::unique_lock<std::mutex> lock(mtx);
            this->sessions_.push_back(session);
        }
        LOG("创建Session成功,目标ip：" + ip + "\n", INFO);
    }catch (const std::exception& e) {
        LOG(std::string("创建Session失败: ") + e.what(), ERROR);
        throw std::runtime_error("创建Session失败");
    }
}

TcpModule::TcpModule()
    : socket_(io_context_),
      work_guard_(boost::asio::make_work_guard(io_context_)) // 直接构造 work_guard

{
    
    init();
    //向线程池提交三个运行io的线程
    ThreadPool::getInstance().enqueue(&TcpModule::io_run,shared_from_this());
    ThreadPool::getInstance().enqueue(&TcpModule::io_run,shared_from_this());
    ThreadPool::getInstance().enqueue(&TcpModule::io_run,shared_from_this());
}
void TcpModule::init(){
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
void TcpModule::rebind(const std::string &ip)
{
    try{
        //拿锁
        std::unique_lock<std::mutex> lock(mtx);
        //遍历sessions_
        for(auto& session : this->sessions_){
            //关闭socket
            session->stop();
        }
        //清空sessions_
        this->sessions_.clear();
        lock.unlock();
        //重新绑定socket
        this->socket_.close();
        this->socket_.open(boost::asio::ip::tcp::v4());//打开socket
        this->socket_.set_option(boost::asio::ip::tcp::socket::reuse_address(true));//设置socket的选项
        //获取当前适配器
        auto adp = NetworkHelper::getInstance().getAdapterInfo().getNowAdapter();
        //初始化一个endpoint
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(adp.ipv4.c_str()), this->port_);
        socket_.bind(endpoint);//绑定socket到endpoint
        LOG("tcp已重新绑定socket到ip: " + ip, INFO);
    }catch(const std::exception& e){
        LOG(std::string("rebind失败: ") + e.what(), ERROR);
        throw std::runtime_error("rebind失败"); // 可选：向上传递异常或处理
    }
}
void TcpModule::connect_by_udpEndpoint(std::unordered_set<boost::asio::ip::udp::endpoint> endpoints)
{
    //发送自身的适配器和名称等信息
    UserInfoAndAdapterInfo info;
    info.userInfo = UserInfoHelper::getInstance().getUserInfo();
    info.adapterInfo = NetworkHelper::getInstance().getAdapterInfo().getNowAdapter();
    //序列化
    auto data = info.serialize();
    try{
        //遍历endpoints
        for(auto& endpoint : endpoints){
            createSession(endpoint.address().to_string());
        }
        sendDataToAll(std::move(data));//发送数据到所有连接的客户端,此时所有客户端都已经connecte
    }catch(const std::exception& e){
        LOG(std::string("connect_by_udpEndpoint失败: ") + e.what(), ERROR);
        throw std::runtime_error("connect_by_udpEndpoint失败"); // 可选：向上传递异常或处理
    }
}
void TcpModule::sendData(const std::string &ip, std::vector<char> &&data)
{
    try{
        {
            std::unique_lock<std::mutex> lock(mtx);
            //遍历sessions_
            for(auto& session : this->sessions_){
                //如果ip相同，发送数据
                if(session->get_ip() == ip){
                    session->dowrite(std::move(data));
                    return;
                }
            }
        }
    }catch(const std::exception& e){
        LOG(std::string("sendData失败: ") + e.what(), ERROR);
        throw std::runtime_error("sendData失败"); // 可选：向上传递异常或处理
    }
}
void TcpModule::sendDataToAll(std::vector<char> &&data)
{
    try{
        std::unique_lock<std::mutex> lock(mtx);
        //遍历sessions_
        for(auto& session : this->sessions_){
            //发送数据
            session->dowrite(std::move(data));
        }
    }catch(const std::exception& e){
        LOG(std::string("sendDataToAll失败: ") + e.what(), ERROR);
    }
}
/*-------Session类-----------*/
void Session::doread()
{
    // 确保 recv_buffer_ 准备好接收数据
    // 如果 recv_buffer_ 是 std::vector<char> 并且您想覆盖旧数据，
    // 并且确保它有足够的大小。通常在创建时或每次读取前调整。
    // 例如，如果 recv_buffer_ 还没有分配足够的空间：
    if (recv_buffer_.size() < 1024 * 9)
    {
        recv_buffer_.resize(1024 * 9);
    }

    auto self = shared_from_this(); // 获取当前对象的 shared_ptr
    socket_.async_read_some(
        boost::asio::buffer(recv_buffer_.data(), recv_buffer_.size()), // 使用 .data() 和 .size() 更安全
        [this, self](const boost::system::error_code &error, std::size_t bytes_transferred)
        {
            if (!error)
            {
                // 成功读取 bytes_transferred 字节的数据
                LOG(std::string("读取到 ") + std::to_string(bytes_transferred) + " 字节数据", INFO);
                std::vector<char> data;
                {
                    // 加锁，确保在读取数据时不会被其他线程修改
                    std::unique_lock<std::mutex> lock(recv_mtx);
                    // 处理数据
                    data = std::vector<char>(recv_buffer_.begin(), recv_buffer_.begin() + bytes_transferred);
                }
                // TODO: 在这里处理接收到的数据
                // 例如: handel_data(recv_buffer_.data(), bytes_transferred);
                // 注意：handel_data 需要接收实际读取的数据量
                // int result = handel_data(); // 您当前的 handel_data 不接收参数

                // 调用 on_data_control_ 处理数据
                if (on_data_control_)
                {
                    on_data_control_(std::move(data)); // 调用 on_data_control_ 处理数据
                }
                else
                {
                    LOG("未设置 on_data_control_ 回调", WARNING);
                }

                // 处理完数据后，继续下一次读取
                doread();
            }
            else
            {
                // 发生错误
                if (error == boost::asio::error::eof)
                {
                    LOG("连接被对方关闭 (EOF)", INFO);
                }
                else if (error == boost::asio::error::operation_aborted)
                {
                    LOG("读取操作被取消 (aborted)", INFO);
                }
                else
                {
                    LOG(std::string("read失败: ") + error.message(), ERROR);
                }
                // 发生错误或EOF，关闭会话
                // 通常在这里调用 stop() 或者通知 TcpModule 移除此会话
                // 例如: stop();
                // 或者: if (auto owner = owner_module_.lock()) { owner->remove_session(self); }
                // 这里简单地停止，具体实现取决于您的会话管理策略
                stop(); // 确保 stop() 能被安全调用
            }
        });
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

/*-------UdpModule类-----------*/
void UdpModule::handel_receive(boost::asio::ip::udp::endpoint remote_endpoint)
{
    //判断这个remote_endpoint是否在endpoints_中
    if(this->endpoints_.find(remote_endpoint) == this->endpoints_.end()){
        //如果不在，就添加
        this->endpoints_.insert(remote_endpoint);
    }
}

void UdpModule::start_recv()
{

    this->socket_.async_receive_from(
        boost::asio::buffer(this->recv_buffer_, 1024 * 9),
        this->remote_endpoint_,
        [this](const boost::system::error_code &error, std::size_t bytes_transferred)
        {
            if (error)
            {
                LOG(std::string("recv失败: ") + error.message(), ERROR);
                throw std::runtime_error("recv失败"); // 可选：向上传递异常或处理
            }

            // 处理数据
            handel_receive(this->remote_endpoint_);
            start_recv();
        });
}

void UdpModule::broadcast()
{
    int count = 0;
    Info info;
    info.taskType = UDP_BROADCAST;
    info.data.resize(0);
    auto task_data = info.serialize();
    //初始化boostbuffer
    boost::asio::const_buffer buffer(task_data.data(), task_data.size());
    //获取适配器信息
    auto adp = NetworkHelper::getInstance().getAdapterInfo().getNowAdapter();
    try{
        //广播三次
        while (count < 3)
        {
            socket_.send_to(buffer, boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(adp.ipv4.c_str()), UDP_PORT));
            //休眠3秒
            std::this_thread::sleep_for(std::chrono::seconds(1));
            count++;
        }
    }catch( std::exception& e){
        LOG(std::string("广播失败: ") + e.what(), ERROR);
        throw std::runtime_error("广播失败"); // 可选：向上传递异常或处理
    }
    //等待2秒确认收到回复
    std::this_thread::sleep_for(std::chrono::seconds(2));
    //将tcp连接任务提交到线程池
    ThreadPool::getInstance().enqueue(
      &TcpModule::connect_by_udpEndpoint,
      this->tcpModule,
      this->endpoints_
    );
}

void UdpModule::rebind(const std::string &ip){
    try{
        //先关闭自身socket，然后重新绑定
        this->socket_.close();
        this->socket_.open(boost::asio::ip::udp::v4());//打开socket
        this->socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));//设置socket的选项
        this->socket_.set_option(boost::asio::socket_base::broadcast(true));//启用广播
        //获取当前适配器
        auto adp = NetworkHelper::getInstance().getAdapterInfo().getNowAdapter();
        //初始化一个endpoint
        boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::address::from_string(adp.ipv4.c_str()), UDP_PORT);
        socket_.bind(endpoint);//绑定socket到endpoint
        LOG("udp已重新绑定socket到ip: " + ip, INFO);
        this->tcpModule.rebind(ip);
        LOG("tcp重绑准备: " + ip, INFO);
    }catch( std::exception& e){
        LOG(std::string("rebind失败: ") + e.what(), ERROR);
        throw std::runtime_error("rebind失败"); // 可选：向上传递异常或处理
    }
}
void UdpModule::init(){
    socket_.open(boost::asio::ip::udp::v4());//打开socket
    socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));//设置socket的选项
    socket_.set_option(boost::asio::socket_base::broadcast(true));//启用广播
    //获取当前在线的adapter
    auto adp = NetworkHelper::getInstance().getAdapterInfo().getNowAdapter();
    //初始化一个endpoint
    boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::address::from_string(adp.ipv4.c_str()), UDP_PORT);
    socket_.bind(endpoint);//绑定socket到endpoint
}
UdpModule::~UdpModule() {
    work_guard_.reset();  // 若有 work_guard_ 成员（需补充）
    io_context_.stop();
    socket_.close();
    
}