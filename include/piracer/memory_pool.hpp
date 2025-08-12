#pragma once
#include <vector>
#include <memory>
#include <cstddef>

namespace piracer {
    
    // Memory pool for efficient allocation of large numbers
    // Reduces malloc overhead and improves cache locality
    
    class MemoryPool {
    public:
        explicit MemoryPool(size_t initial_size = 1024 * 1024);  // 1MB default
        ~MemoryPool() = default;
        
        // Disable copy
        MemoryPool(const MemoryPool&) = delete;
        MemoryPool& operator=(const MemoryPool&) = delete;
        
        // Allocate memory
        void* allocate(size_t size);
        
        // Deallocate memory (no-op for pool)
        void deallocate(void* ptr, size_t size);
        
        // Get pool statistics
        size_t total_allocated() const { return total_allocated_; }
        size_t total_used() const { return total_used_; }
        size_t pool_count() const { return pools.size(); }
        
        // Clear all pools
        void clear();
        
        // Reserve additional memory
        void reserve(size_t size);
        
    private:
        struct Pool {
            std::vector<uint8_t> data;
            size_t used = 0;
            
            Pool(size_t size) : data(size) {}
            
            bool can_allocate(size_t size) const {
                return used + size <= data.size();
            }
            
            void* allocate(size_t size) {
                if (!can_allocate(size)) return nullptr;
                void* ptr = data.data() + used;
                used += size;
                return ptr;
            }
        };
        
        std::vector<std::unique_ptr<Pool>> pools;
        size_t total_allocated_ = 0;
        size_t total_used_ = 0;
        
        // Find or create pool that can fit the request
        Pool* find_or_create_pool(size_t size);
    };
    
    // Custom allocator using memory pool
    template<typename T>
    class PoolAllocator {
    public:
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        
        explicit PoolAllocator(MemoryPool* pool) : pool_(pool) {}
        
        template<typename U>
        PoolAllocator(const PoolAllocator<U>& other) : pool_(other.pool_) {}
        
        T* allocate(size_type n) {
            size_type bytes = n * sizeof(T);
            return static_cast<T*>(pool_->allocate(bytes));
        }
        
        void deallocate(T* p, size_type n) {
            // No-op for pool allocator
            (void)p; (void)n;
        }
        
        template<typename U, typename... Args>
        void construct(U* p, Args&&... args) {
            ::new(p) U(std::forward<Args>(args)...);
        }
        
        template<typename U>
        void destroy(U* p) {
            p->~U();
        }
        
        MemoryPool* pool() const { return pool_; }
        
    private:
        MemoryPool* pool_;
    };
    
    // Global memory pool instance
    extern MemoryPool g_memory_pool;
    
    // Convenience functions
    template<typename T>
    using pool_vector = std::vector<T, PoolAllocator<T>>;
    
    template<typename T>
    pool_vector<T> make_pool_vector(size_t size = 0) {
        return pool_vector<T>(size, PoolAllocator<T>(&g_memory_pool));
    }
    
} // namespace piracer 