#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>

namespace piracer {
    
    // Efficient thread pool for parallel binary-splitting
    class ThreadPool {
    public:
        explicit ThreadPool(size_t num_threads);
        ~ThreadPool();
        
        // Disable copy
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;
        
        // Submit a task and return a future
        template<class F, class... Args>
        auto submit(F&& f, Args&&... args) 
            -> std::future<typename std::result_of<F(Args...)>::type>;
        
        // Get number of threads
        size_t size() const { return workers.size(); }
        
        // Wait for all tasks to complete
        void wait_all();
        
    private:
        // Worker threads
        std::vector<std::thread> workers;
        
        // Task queue
        std::queue<std::function<void()>> tasks;
        
        // Synchronization
        mutable std::mutex queue_mutex;
        std::condition_variable condition;
        
        // Stop flag
        bool stop;
        
        // Worker function
        void worker_function();
    };
    
    // Implementation
    template<class F, class... Args>
    auto ThreadPool::submit(F&& f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type> {
        
        using return_type = typename std::result_of<F(Args...)>::type;
        
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        std::future<return_type> res = task->get_future();
        
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            if (stop) {
                throw std::runtime_error("submit on stopped ThreadPool");
            }
            tasks.emplace([task](){ (*task)(); });
        }
        
        condition.notify_one();
        return res;
    }
    
} // namespace piracer 