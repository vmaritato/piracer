#pragma once
#include <string>
#include <memory>
#include <functional>
#include <map>
#include "piracer/simd.hpp"
#include "piracer/gpu_backend.hpp"

namespace piracer {

    // Algorithm types
    enum class AlgorithmType {
        CHUDNOVSKY,
        BINARY_SPLITTING,
        CHUDNOVSKY_PARALLEL,
        CHUDNOVSKY_SIMD,
        CHUDNOVSKY_GPU,
        AUTO  // Automatically select best available
    };

    // Algorithm configuration
    struct AlgorithmConfig {
        AlgorithmType type = AlgorithmType::AUTO;
        bool use_simd = true;
        bool use_gpu = false;
        int num_threads = 1;
        size_t gpu_memory_limit_mb = 1024;  // 1GB default
        bool enable_profiling = false;
        std::string output_format = "decimal";
        
        // SIMD specific options
        bool prefer_avx512 = true;
        bool prefer_neon = true;
        
        // GPU specific options
        GPUBackend preferred_gpu = GPUBackend::Auto;
        bool fallback_to_cpu = true;
    };

    // Algorithm interface
    class Algorithm {
    public:
        virtual ~Algorithm() = default;
        
        // Compute π with given precision
        virtual std::string compute_pi(size_t digits) = 0;
        
        // Get algorithm name
        virtual std::string get_name() const = 0;
        
        // Get algorithm type
        virtual AlgorithmType get_type() const = 0;
        
        // Get performance metrics
        virtual double get_performance_ns_per_digit() const = 0;
        virtual size_t get_memory_usage_mb() const = 0;
        virtual double get_total_time_ms() const = 0;
        
        // Check if algorithm supports given configuration
        virtual bool supports_config(const AlgorithmConfig& config) const = 0;
        
        // Get optimal configuration for given digits
        virtual AlgorithmConfig get_optimal_config(size_t digits) const = 0;
    };

    // Chudnovsky algorithm with SIMD optimization
    class ChudnovskySIMD : public Algorithm {
    public:
        explicit ChudnovskySIMD(const AlgorithmConfig& config);
        ~ChudnovskySIMD() override = default;
        
        std::string compute_pi(size_t digits) override;
        std::string get_name() const override { return "Chudnovsky-SIMD"; }
        AlgorithmType get_type() const override { return AlgorithmType::CHUDNOVSKY_SIMD; }
        
        double get_performance_ns_per_digit() const override { return performance_ns_per_digit_; }
        size_t get_memory_usage_mb() const override { return memory_usage_mb_; }
        double get_total_time_ms() const override { return total_time_ms_; }
        
        bool supports_config(const AlgorithmConfig& config) const override;
        AlgorithmConfig get_optimal_config(size_t digits) const override;
        
    private:
        AlgorithmConfig config_;
        simd::CPUFeatures cpu_features_;
        double performance_ns_per_digit_ = 0.0;
        size_t memory_usage_mb_ = 0;
        double total_time_ms_ = 0.0;
        
        // SIMD-optimized computation methods
        void compute_with_avx512(size_t digits);
        void compute_with_neon(size_t digits);
        void compute_with_sse(size_t digits);
        void compute_fallback(size_t digits);
    };

    // Chudnovsky algorithm with GPU acceleration
    class ChudnovskyGPU : public Algorithm {
    public:
        explicit ChudnovskyGPU(const AlgorithmConfig& config);
        ~ChudnovskyGPU() override = default;
        
        std::string compute_pi(size_t digits) override;
        std::string get_name() const override { return "Chudnovsky-GPU"; }
        AlgorithmType get_type() const override { return AlgorithmType::CHUDNOVSKY_GPU; }
        
        double get_performance_ns_per_digit() const override { return performance_ns_per_digit_; }
        size_t get_memory_usage_mb() const override { return memory_usage_mb_; }
        double get_total_time_ms() const override { return total_time_ms_; }
        
        bool supports_config(const AlgorithmConfig& config) const override;
        AlgorithmConfig get_optimal_config(size_t digits) const override;
        
    private:
        AlgorithmConfig config_;
        std::unique_ptr<GPUContext> gpu_context_;
        std::unique_ptr<GPUMultiplier> gpu_multiplier_;
        double performance_ns_per_digit_ = 0.0;
        size_t memory_usage_mb_ = 0;
        double total_time_ms_ = 0.0;
        
        // GPU-optimized computation methods
        void compute_with_cuda(size_t digits);
        void compute_with_opencl(size_t digits);
        void compute_fallback_to_cpu(size_t digits);
        
        // Check GPU availability and select best backend
        bool initialize_gpu_backend();
    };

    // Algorithm factory for creating optimal algorithms
    class AlgorithmFactory {
    public:
        // Create algorithm with optimal configuration
        static std::unique_ptr<Algorithm> create_algorithm(
            size_t digits, 
            const AlgorithmConfig& config = AlgorithmConfig{});
        
        // Get available algorithms for given configuration
        static std::vector<AlgorithmType> get_available_algorithms(
            const AlgorithmConfig& config);
        
        // Benchmark all available algorithms
        static std::map<AlgorithmType, double> benchmark_algorithms(
            size_t digits, 
            int iterations = 3);
        
        // Get algorithm recommendations
        struct AlgorithmRecommendation {
            AlgorithmType best_algorithm;
            AlgorithmConfig optimal_config;
            double expected_performance_ns_per_digit;
            std::string reasoning;
        };
        
        static AlgorithmRecommendation get_recommendation(
            size_t digits, 
            const AlgorithmConfig& preferences = AlgorithmConfig{});
        
        // Check system capabilities
        static bool has_simd_support();
        static bool has_gpu_support();
        static std::vector<std::string> get_system_capabilities();
        
    private:
        // Algorithm selection logic
        static AlgorithmType select_best_algorithm(
            size_t digits, 
            const AlgorithmConfig& config);
        
        // Configuration optimization
        static AlgorithmConfig optimize_config(
            size_t digits, 
            const AlgorithmConfig& base_config);
        
        // Performance prediction
        static double predict_performance(
            AlgorithmType algo, 
            size_t digits, 
            const AlgorithmConfig& config);
    };

    // Performance comparison utility
    class AlgorithmComparator {
    public:
        // Compare two algorithms
        struct ComparisonResult {
            std::string algorithm_a_name;
            std::string algorithm_b_name;
            double speedup_factor;
            double memory_ratio;
            std::vector<std::string> advantages_a;
            std::vector<std::string> advantages_b;
            std::string recommendation;
        };
        
        static ComparisonResult compare_algorithms(
            std::unique_ptr<Algorithm> algo_a,
            std::unique_ptr<Algorithm> algo_b,
            size_t digits,
            int iterations = 3);
        
        // Generate comparison report
        static std::string generate_comparison_report(
            const std::vector<ComparisonResult>& results);
        
        // Export comparison to CSV
        static bool export_comparison_to_csv(
            const std::string& filename,
            const std::vector<ComparisonResult>& results);
        
    private:
        static double measure_algorithm_performance(
            Algorithm* algo, 
            size_t digits, 
            int iterations);
    };

} // namespace piracer 