#include "piracer/thread_pool.hpp"
#include <algorithm>

namespace piracer {
    
    ThreadPool::ThreadPool(size_t num_threads) : stop(false) {
        // Create worker threads
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back(&ThreadPool::worker_function, this);
        }
    }
    
    ThreadPool::~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            stop = true;
        }
        
        condition.notify_all();
        
        // Join all threads
        for (std::thread& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
    
    void ThreadPool::worker_function() {
        while (true) {
            std::function<void()> task;
            
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                
                // Wait for task or stop signal
                condition.wait(lock, [this] { 
                    return stop || !tasks.empty(); 
                });
                
                if (stop && tasks.empty()) {
                    return;
                }
                
                if (!tasks.empty()) {
                    task = std::move(tasks.front());
                    tasks.pop();
                }
            }
            
            if (task) {
                task();
            }
        }
    }
    
    void ThreadPool::wait_all() {
        // Wait for all tasks to complete
        while (true) {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (tasks.empty()) {
                break;
            }
            lock.unlock();
            std::this_thread::yield();
        }
    }
    
} // namespace piracer 