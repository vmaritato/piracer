#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace piracer {

    // GPU backend types
    enum class GPUBackend {
        CUDA,
        OpenCL,
        Auto  // Automatically select best available
    };

    // GPU device information
    struct GPUDevice {
        std::string name;
        std::string vendor;
        size_t memory_mb;
        int compute_capability_major;
        int compute_capability_minor;
        bool supports_double_precision;
        size_t max_work_group_size;
        
        GPUDevice() = default;
        GPUDevice(const std::string& n, const std::string& v, size_t mem, 
                  int cc_major, int cc_minor, bool fp64, size_t wg_size)
            : name(n), vendor(v), memory_mb(mem), compute_capability_major(cc_major),
              compute_capability_minor(cc_minor), supports_double_precision(fp64),
              max_work_group_size(wg_size) {}
    };

    // GPU computation context
    class GPUContext {
    public:
        virtual ~GPUContext() = default;
        
        // Initialize GPU context
        virtual bool initialize() = 0;
        
        // Get device information
        virtual std::vector<GPUDevice> get_devices() const = 0;
        
        // Select device
        virtual bool select_device(int device_id) = 0;
        
        // Get selected device
        virtual GPUDevice get_selected_device() const = 0;
        
        // Check if context is valid
        virtual bool is_valid() const = 0;
    };

    // GPU multiplication backend
    class GPUMultiplier {
    public:
        virtual ~GPUMultiplier() = default;
        
        // Multiply two large integers (represented as byte arrays)
        virtual std::vector<uint8_t> multiply(const std::vector<uint8_t>& a, 
                                            const std::vector<uint8_t>& b) = 0;
        
        // Multiply multiple pairs at once (batch operation)
        virtual std::vector<std::vector<uint8_t>> multiply_batch(
            const std::vector<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>>& pairs) = 0;
        
        // Get performance metrics
        virtual double get_multiplication_time_ms() const = 0;
        virtual size_t get_memory_usage_mb() const = 0;
        
        // Benchmark operations
        virtual void benchmark(size_t digit_count, int iterations = 100) = 0;
    };

    // GPU backend factory
    class GPUBackendFactory {
    public:
        // Create GPU context
        static std::unique_ptr<GPUContext> create_context(GPUBackend backend = GPUBackend::Auto);
        
        // Create GPU multiplier
        static std::unique_ptr<GPUMultiplier> create_multiplier(GPUBackend backend = GPUBackend::Auto);
        
        // Check if backend is available
        static bool is_backend_available(GPUBackend backend);
        
        // Get best available backend
        static GPUBackend get_best_available_backend();
        
        // Get backend information
        static std::string get_backend_info(GPUBackend backend);
    };

    // GPU performance profiler
    class GPUProfiler {
    public:
        GPUProfiler() = default;
        ~GPUProfiler() = default;
        
        // Start profiling
        void start_profiling();
        
        // Stop profiling
        void stop_profiling();
        
        // Get profiling results
        struct ProfileResult {
            double total_time_ms;
            double kernel_time_ms;
            double memory_transfer_time_ms;
            size_t memory_allocated_mb;
            size_t operations_performed;
            double throughput_gflops;
        };
        
        ProfileResult get_results() const;
        
        // Reset profiler
        void reset();
        
        // Export results to CSV
        bool export_to_csv(const std::string& filename) const;
        
    private:
        ProfileResult current_result_;
        bool is_profiling_ = false;
        std::chrono::high_resolution_clock::time_point start_time_;
    };

    // GPU memory manager
    class GPUMemoryManager {
    public:
        GPUMemoryManager() = default;
        ~GPUMemoryManager() = default;
        
        // Allocate GPU memory
        template<typename T>
        T* allocate_gpu(size_t count);
        
        // Free GPU memory
        template<typename T>
        void free_gpu(T* ptr);
        
        // Copy data to GPU
        template<typename T>
        void copy_to_gpu(const std::vector<T>& host_data, T* gpu_ptr);
        
        // Copy data from GPU
        template<typename T>
        void copy_from_gpu(T* gpu_ptr, std::vector<T>& host_data);
        
        // Get memory usage statistics
        size_t get_total_allocated_mb() const;
        size_t get_peak_usage_mb() const;
        size_t get_allocation_count() const;
        
        // Clear all allocations
        void clear_all();
        
    private:
        struct Allocation {
            void* ptr;
            size_t size;
            std::chrono::system_clock::time_point timestamp;
        };
        
        std::vector<Allocation> allocations_;
        size_t total_allocated_ = 0;
        size_t peak_usage_ = 0;
    };

} // namespace piracer 