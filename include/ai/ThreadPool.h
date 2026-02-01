#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>

namespace ConnectFour {

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads = 0);
    ~ThreadPool();
    
    // Submit a task and get a future for the result
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type>;
    
    // Wait for all tasks to complete
    void waitAll();
    
    // Get number of threads
    size_t getNumThreads() const { return workers_.size(); }
    
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    
    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::atomic<bool> stop_{false};
    std::atomic<int> activeTasks_{0};
    std::condition_variable allTasksDone_;
    
    void workerThread();
};

// Template implementation must be in header
template<typename F, typename... Args>
auto ThreadPool::submit(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type> {
    using return_type = typename std::invoke_result<F, Args...>::type;
    
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    
    std::future<return_type> result = task->get_future();
    
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        if (stop_) {
            throw std::runtime_error("Cannot submit task to stopped ThreadPool");
        }
        
        tasks_.emplace([task, this]() {
            activeTasks_++;
            (*task)();
            activeTasks_--;
            allTasksDone_.notify_all();
        });
    }
    
    condition_.notify_one();
    return result;
}

} // namespace ConnectFour
