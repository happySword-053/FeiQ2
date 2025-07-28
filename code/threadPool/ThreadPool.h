#pragma once
#include<thread>
#include<functional>
#include<mutex>
#include<queue>
#include<condition_variable>
#include<vector>
#include<atomic>
#include<memory>
#include<future>
#include<type_traits>
#include<future>
#include<list>
#include"../logs/logs.h"

// 线程池基类
class BaseThreadPool {
protected:
    std::vector<std::thread> workers;       // 工作线程
    std::queue<std::function<void()>> tasks; // 任务队列
    std::mutex queue_mutex;                 // 队列互斥锁
    std::condition_variable condition;      // 条件变量
    std::atomic<bool> stop;                 // 停止标志
    std::atomic<int> threadCount;           // 当前线程数
    std::atomic<int> maxThreadCount;        // 最大线程数
    std::atomic<int> minThreadCount;        // 最小线程数

public:
    // 构造函数
    BaseThreadPool(int minThread = 1, int maxThread = 3)
        : minThreadCount(minThread),
          maxThreadCount(maxThread),
          stop(false),
          threadCount(0) {}

    // 禁止拷贝构造和赋值
    BaseThreadPool(const BaseThreadPool&) = delete;
    BaseThreadPool& operator=(const BaseThreadPool&) = delete;

    // 纯虚函数：初始化线程池（由派生类实现）
    virtual int init() = 0;
    // 线程任务函数
    virtual void threadTask() = 0;
    // 任务入队（通用实现）
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
        using return_type = typename std::result_of<F(Args...)>::type;
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (stop) {
                throw std::runtime_error("线程池已停止，无法添加任务");
            }
            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return res;
    }

    // 虚析构函数
    virtual ~BaseThreadPool() {
        stop = true;
        condition.notify_all();
        for (std::thread &worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
};

// 文件系统线程池（最多3个线程） 单独设计，不继承基类线程池

class FileSystemThreadPool {
private:
    std::atomic<int> max_threads;
    std::atomic<int> active_threads{0};
    
    struct ThreadState {
        std::thread thread;
        std::atomic<bool> running{false};
        std::atomic<bool> should_stop{false};
        
        // 禁止在析构中join，由线程池统一管理
        ~ThreadState() {
            if (thread.joinable()) {
                thread.detach(); // 安全分离
            }
        }
    };
    
    std::list<std::unique_ptr<ThreadState>> threads;
    std::unordered_map<std::thread::id, ThreadState*> thread_map;
    
    std::condition_variable cv;
    std::atomic<bool> stop_flag{false};
    std::queue<std::function<void()>> tasks;
    
    std::mutex global_mutex; // 单锁简化设计
    
    // 获取忙碌线程数
    int getBusyThreadCount() {
        int count = 0;
        {
            std::unique_lock<std::mutex> lock(global_mutex);
            
            for (const auto& state : threads) {
                if (state->running.load()) {
                    ++count;
                }
            }
        }
        return count;
    }
    
    void threadTask() {
        ThreadState* my_state = nullptr;
        {
            std::unique_lock<std::mutex> lock(global_mutex);
            auto it = thread_map.find(std::this_thread::get_id());
            if (it == thread_map.end()) {
                return; // 无效线程
            }
            my_state = it->second;
            my_state->running = false;
        }
        
        while (true) {
            std::function<void()> task;
            bool should_exit = false;
            
            {
                std::unique_lock<std::mutex> lock(global_mutex);
                
                // 精确等待条件：任务/停止信号/线程退出指令
                cv.wait(lock, [&] {
                    return stop_flag || !tasks.empty() || 
                           my_state->should_stop.load() ||
                           active_threads > max_threads;
                });
                
                // 检查退出条件
                if ((stop_flag && tasks.empty()) || my_state->should_stop || active_threads > max_threads) {
                    should_exit = true;
                    break;
                }
                
                // 取任务
                if (!should_exit && !tasks.empty()) {
                    task = std::move(tasks.front());
                    tasks.pop();
                }
            }
            
            if (should_exit) break;
            
            // 执行任务（在锁外）
            if (task) {
                my_state->running = true;
                try {
                    task();
                } catch (...) {
                    // 异常处理
                }
                my_state->running = false;
            }
        }
        
        // 标记线程结束
        {
            std::lock_guard<std::mutex> lock(global_mutex);
            thread_map.erase(std::this_thread::get_id());
            active_threads--;
        }
        cv.notify_all(); // 通知线程数量变化
    }
    
    void addThread() {
        std::lock_guard<std::mutex> lock(global_mutex);
        
        if (active_threads >= max_threads) return;
        
        auto state = std::make_unique<ThreadState>();
        state->running = false;
        state->should_stop = false;
        
        try {
            state->thread = std::thread(&FileSystemThreadPool::threadTask, this);
        } catch (...) {
            // 线程创建失败处理
            return;
        }
        
        auto* raw_ptr = state.get();
        threads.push_back(std::move(state));
        thread_map[raw_ptr->thread.get_id()] = raw_ptr;
        active_threads++;
    }
    
    void markThreadForExit(int num) {
        std::lock_guard<std::mutex> lock(global_mutex);
        if (active_threads <= max_threads) return;
        
        // 查找可退出的空闲线程
        for (auto& state : threads) {
            if (!state->running && !state->should_stop && num > 0) {
                state->should_stop = true;
                num--;
            }
        }
        
        // 没有空闲线程，随机标记一个非should_stop的线程
        if (!threads.empty()) {
            while(num > 0){
                for (auto& state : threads) {
                    if (!state->should_stop) {
                        state->should_stop = true;
                        num--;
                    }
                }
            }
        }
    }
    FileSystemThreadPool(int init_threads)
            : max_threads(init_threads) {
            for (int i = 0; i < init_threads; ++i) {
                addThread();
            }
        }
public:
    

    ~FileSystemThreadPool() {
        stop_flag = true;
        {
            std::lock_guard<std::mutex> lock(global_mutex);
            // 标记所有线程退出
            for (auto& state : threads) {
                state->should_stop = true;
            }
        }
        
        cv.notify_all(); // 唤醒所有线程
        
        // 等待所有线程结束
        for (auto& state : threads) {
            if (state->thread.joinable()) {
                state->thread.join();
            }
        }
    }

    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
        using return_type = decltype(f(args...));
        
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::lock_guard<std::mutex> lock(global_mutex);
            if (stop_flag) {
                throw std::runtime_error("Enqueue on stopped ThreadPool");
            }
            tasks.emplace([task] { (*task)(); });
        }
        cv.notify_one();
        return res;
    }

    void setThreadCount(int num) {
        if (num < 1) num = 1;
        max_threads = num;
        
        // 增加线程
        if(active_threads < max_threads){
            while (active_threads < max_threads) {
                addThread();
            }
        }      
        else
        {
            // 减少线程
            markThreadForExit(active_threads - max_threads);   
            cv.notify_all();
        }
    }

    // 获取单例
    static FileSystemThreadPool& getInstance(int num = 1) {
        static FileSystemThreadPool instance(num);
        return instance;
    }
};

// 网络线程池（单例模式）
class NetThreadPool : public BaseThreadPool {
private:
    std::atomic<int> idleThreadCount;  // 空闲线程数量
    std::atomic<int> busyThreadCount;  // 忙线程数量
    std::atomic<int> taskCount;        // 任务数量

    // 私有构造函数
    NetThreadPool(int minThreadCount = 10, int maxThreadCount = 100)
        : BaseThreadPool(minThreadCount, maxThreadCount),
          idleThreadCount(0),
          busyThreadCount(0),
          taskCount(0) {}

public:
    // 单例实例获取
    static NetThreadPool& getInstance(int minThread = 10, int maxThread = 100) {
        static NetThreadPool instance(minThread, maxThread);
        return instance;
    }

    // 禁止拷贝构造和赋值
    NetThreadPool(const NetThreadPool&) = delete;
    NetThreadPool& operator=(const NetThreadPool&) = delete;
    void threadTask() override { 
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                idleThreadCount++;
                condition.wait(lock, [this] { return stop || !tasks.empty(); });
                idleThreadCount--;

                if (stop && tasks.empty()) return;
                task = std::move(tasks.front());
                tasks.pop();
                busyThreadCount++;
                taskCount++;
            }
            task();
            busyThreadCount--;
        }
    }
    // 初始化线程池
    int init() override {
        for (int i = 0; i < minThreadCount; ++i) {
            workers.emplace_back(&NetThreadPool::threadTask, this);
        }
        threadCount.store(minThreadCount);
        return 0;
    }

    // 网络线程池特有方法
    int getBusyThreadCount() { return busyThreadCount.load(); }

    // 获取单例
    
};

// 通用任务线程池
class CurrentThreadPool : public BaseThreadPool {
private:
    std::atomic<int> idleThreadCount;  // 空闲线程数量
    std::atomic<int> busyThreadCount;  // 忙线程数量
    std::atomic<int> taskCount;        // 任务数量

    // 私有构造函数
    CurrentThreadPool(int minThreadCount = 1, int maxThreadCount = 3)
        : BaseThreadPool(minThreadCount, maxThreadCount),
          idleThreadCount(0),
          busyThreadCount(0),
          taskCount(0) {}

public:
    // 单例实例获取
    static CurrentThreadPool& getInstance(int minThread = 10, int maxThread = 100) {
        static CurrentThreadPool instance(minThread, maxThread);
        return instance;
    }

    // 禁止拷贝构造和赋值
    CurrentThreadPool(const NetThreadPool&) = delete;
    CurrentThreadPool& operator=(const NetThreadPool&) = delete;
    void threadTask() override { 
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                idleThreadCount++;
                condition.wait(lock, [this] { return stop || !tasks.empty(); });
                idleThreadCount--;

                if (stop && tasks.empty()) return;
                task = std::move(tasks.front());
                tasks.pop();
                busyThreadCount++;
                taskCount++;
            }
            task();
            busyThreadCount--;
        }
    }
    // 初始化线程池
    int init() override {
        for (int i = 0; i < minThreadCount; ++i) {
            workers.emplace_back(&NetThreadPool::threadTask, this);
        }
        threadCount.store(minThreadCount);
        return 0;
    }

    // 网络线程池特有方法
    int getBusyThreadCount() { return busyThreadCount.load(); }
     
};