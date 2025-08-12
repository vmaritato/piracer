#pragma once
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <functional>
#include <memory>

namespace piracer {

    // Performance measurement types
    enum class ProfilerMetric {
        CPU_TIME,
        WALL_TIME,
        MEMORY_USAGE,
        CACHE_MISSES,
        BRANCH_MISSES,
        INSTRUCTIONS,
        CYCLES,
        CUSTOM
    };

    // Performance event
    struct PerformanceEvent {
        std::string name;
        std::string category;
        ProfilerMetric metric;
        double value;
        std::string unit;
        std::chrono::system_clock::time_point timestamp;
        std::map<std::string, std::string> metadata;
        
        PerformanceEvent(const std::string& n, const std::string& cat, 
                        ProfilerMetric m, double v, const std::string& u = "")
            : name(n), category(cat), metric(m), value(v), unit(u),
              timestamp(std::chrono::system_clock::now()) {}
    };

    // Performance profiler
    class PerformanceProfiler {
    public:
        PerformanceProfiler() = default;
        ~PerformanceProfiler() = default;
        
        // Start profiling a section
        void start_section(const std::string& name, const std::string& category = "default");
        
        // End profiling a section
        void end_section(const std::string& name);
        
        // Measure a function execution time
        template<typename F, typename... Args>
        auto measure_function(const std::string& name, F&& func, Args&&... args)
            -> decltype(func(std::forward<Args>(args)...);
        
        // Add custom metric
        void add_metric(const std::string& name, ProfilerMetric metric, double value, 
                       const std::string& unit = "", const std::string& category = "default");
        
        // Get profiling results
        struct ProfilingResult {
            std::vector<PerformanceEvent> events;
            std::map<std::string, double> section_times;
            std::map<std::string, std::vector<double>> metric_values;
            double total_time_ms;
            size_t total_events;
        };
        
        ProfilingResult get_results() const;
        
        // Export results to various formats
        bool export_to_csv(const std::string& filename) const;
        bool export_to_json(const std::string& filename) const;
        bool export_to_html(const std::string& filename) const;
        
        // Generate performance report
        std::string generate_report() const;
        
        // Reset profiler
        void reset();
        
        // Get current memory usage
        size_t get_current_memory_usage_mb() const;
        
        // Get peak memory usage
        size_t get_peak_memory_usage_mb() const;
        
    private:
        struct Section {
            std::string name;
            std::string category;
            std::chrono::high_resolution_clock::time_point start_time;
            bool is_active;
        };
        
        std::map<std::string, Section> active_sections_;
        std::vector<PerformanceEvent> events_;
        std::chrono::high_resolution_clock::time_point profiler_start_;
        size_t peak_memory_usage_ = 0;
        
        void update_memory_usage();
    };

    // Cache performance analyzer
    class CacheProfiler {
    public:
        CacheProfiler() = default;
        ~CacheProfiler() = default;
        
        // Analyze cache performance for a data structure
        struct CacheAnalysis {
            size_t total_accesses;
            size_t cache_hits;
            size_t cache_misses;
            double hit_rate;
            double miss_rate;
            size_t cache_line_size;
            size_t total_memory_footprint;
        };
        
        CacheAnalysis analyze_cache_performance(const void* data, size_t size, 
                                              size_t access_pattern = 0);
        
        // Simulate different cache configurations
        std::vector<CacheAnalysis> simulate_cache_configs(const void* data, size_t size);
        
        // Get cache line size
        size_t get_cache_line_size() const;
        
        // Get L1, L2, L3 cache sizes
        size_t get_l1_cache_size() const;
        size_t get_l2_cache_size() const;
        size_t get_l3_cache_size() const;
        
    private:
        size_t cache_line_size_ = 64;  // Default x86_64 cache line size
        size_t l1_cache_size_ = 32 * 1024;   // 32KB
        size_t l2_cache_size_ = 256 * 1024;  // 256KB
        size_t l3_cache_size_ = 8 * 1024 * 1024; // 8MB
    };

    // Branch prediction analyzer
    class BranchProfiler {
    public:
        BranchProfiler() = default;
        ~BranchProfiler() = default;
        
        // Analyze branch prediction performance
        struct BranchAnalysis {
            size_t total_branches;
            size_t taken_branches;
            size_t not_taken_branches;
            size_t mispredicted_branches;
            double prediction_accuracy;
            double taken_rate;
        };
        
        BranchAnalysis analyze_branch_performance(const std::function<void()>& code);
        
        // Get branch prediction statistics
        std::map<std::string, BranchAnalysis> get_branch_statistics() const;
        
        // Reset branch counters
        void reset_counters();
        
    private:
        std::map<std::string, BranchAnalysis> branch_stats_;
    };

    // Performance comparison tool
    class PerformanceComparator {
    public:
        PerformanceComparator() = default;
        ~PerformanceComparator() = default;
        
        // Compare two implementations
        struct ComparisonResult {
            std::string implementation_a;
            std::string implementation_b;
            double speedup_factor;
            double memory_ratio;
            std::vector<std::string> advantages_a;
            std::vector<std::string> advantages_b;
            std::string recommendation;
        };
        
        ComparisonResult compare_implementations(
            const std::string& name_a, const std::function<void()>& impl_a,
            const std::string& name_b, const std::function<void()>& impl_b,
            int iterations = 1000);
        
        // Generate comparison report
        std::string generate_comparison_report(const std::vector<ComparisonResult>& results);
        
        // Export comparison to CSV
        bool export_comparison_to_csv(const std::string& filename, 
                                    const std::vector<ComparisonResult>& results) const;
        
    private:
        double measure_execution_time(const std::function<void()>& func, int iterations);
        size_t measure_memory_usage(const std::function<void()>& func);
    };

    // Global performance profiler instance
    extern std::unique_ptr<PerformanceProfiler> g_profiler;
    
    // Convenience macros for profiling
    #define PROFILE_SECTION(name) \
        piracer::g_profiler->start_section(name); \
        auto _profile_end_##name = [&]() { piracer::g_profiler->end_section(name); }; \
        std::unique_ptr<void, decltype(_profile_end_##name)> _profile_guard_##name(nullptr, _profile_end_##name)
    
    #define PROFILE_FUNCTION() \
        PROFILE_SECTION(__FUNCTION__)

} // namespace piracer 