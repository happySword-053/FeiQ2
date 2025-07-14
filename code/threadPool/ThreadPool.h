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
    BaseThreadPool(int minThread = 10, int maxThread = 100)
        : minThreadCount(minThread),
          maxThreadCount(maxThread),
          stop(false),
          threadCount(0) {}

    // 禁止拷贝构造和赋值
    BaseThreadPool(const BaseThreadPool&) = delete;
    BaseThreadPool& operator=(const BaseThreadPool&) = delete;

    // 纯虚函数：初始化线程池（由派生类实现）
    virtual int init() = 0;

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

// 文件系统线程池（最多3个线程）
class FileSystemThreadPool : public BaseThreadPool {
private:
    std::mutex mapMutex; // 保护映射表的互斥锁
    std::unordered_map<std::string, std::shared_ptr<std::atomic<bool>>> stopFlag; // 文件名到停止标志的映射

public:
    // 构造函数（固定最多3个线程）
    FileSystemThreadPool() : BaseThreadPool(3, 3) {}

    // 初始化线程池
    int init() override {
        for (int i = 0; i < minThreadCount; ++i) {
            workers.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
        threadCount.store(minThreadCount);
        return 0;
    }

    // 文件系统特有方法（保持不变）
    void setStopFlag(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mapMutex);
        auto it = stopFlag.find(filename);
        if (it != stopFlag.end()) {
            *it->second = true;
        }
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

    // 初始化线程池
    int init() override {
        for (int i = 0; i < minThreadCount; ++i) {
            workers.emplace_back([this]() {
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
            });
        }
        threadCount.store(minThreadCount);
        return 0;
    }

    // 网络线程池特有方法
    int getBusyThreadCount() { return busyThreadCount.load(); }
};

// 通用任务线程池
class CrrentThreadPool : public BaseThreadPool {
public:
    // 构造函数
    CrrentThreadPool(int minThread = 10, int maxThread = 100) : BaseThreadPool(minThread, maxThread) {}

    // 初始化线程池
    int init() override {
        for (int i = 0; i < minThreadCount; ++i) {
            workers.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
        threadCount.store(minThreadCount);
        return 0;
    }
};