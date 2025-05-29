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

class ThreadPool {
private:
    // 单例核心：私有构造函数（防止外部实例化）
    ThreadPool(int minThreadCount = 10, int maxThreadCount = 100)
        : minThreadCount(minThreadCount),
          maxThreadCount(maxThreadCount),
          stop(false),
          threadCount(0),
          idleThreadCount(0),
          busyThreadCount(0),
          taskCount(0) {
        // 原构造函数初始化逻辑（需补充线程创建等实现）
        
    }

    // 禁用拷贝/移动操作
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    // 原有成员变量保持不变
    std::vector<std::thread> workers;  // 线程池中的线程
    std::queue<std::function<void()>> tasks;  // 任务队列
    std::mutex queue_mutex;  // 任务队列的互斥锁
    std::condition_variable condition;  // 条件变量，用于线程间的同步
    std::atomic<bool> stop;  // 线程池停止标志
    std::atomic<int> threadCount;  // 线程池中的线程数量
    std::atomic<int> maxThreadCount;  // 线程池中的最大线程数量
    std::atomic<int> minThreadCount;  // 线程池中的最小线程数量
    std::atomic<int> idleThreadCount;  // 空闲线程数量
    std::atomic<int> busyThreadCount;  // 忙线程数量
    std::atomic<int> taskCount;  // 任务数量

public:
    // 单例实例获取方法（线程安全）
    static ThreadPool& getInstance(int minThreadCount = 10, int maxThreadCount = 100) {
        static ThreadPool instance(minThreadCount, maxThreadCount);  // C++11 保证线程安全
        return instance;
    }

    // 添加任务（保持原有逻辑，但通过单例实例调用）
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>{
        using return_type = typename std::result_of<F(Args...)>::type;
        auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (stop) {
                throw std::runtime_error("此时线程池已停止，不能再添加任务");
            }
            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return res;
    }

    int getBusyThreadCount() {
        // 原有实现（需补充）
        return busyThreadCount.load();
    }

    ~ThreadPool() {
        // 原有析构逻辑（需补充线程清理等）
    }
    int init(){
        //初始化线程池
        for (int i = 0; i < minThreadCount; ++i) {
            workers.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) {
                            throw std::runtime_error("解锁的时候此时线程池已经停止了，但是任务队列不为空");
                        }
                        task = std::move(tasks.front());
                    } 
                    task();
                }
            });
        }
    }
};