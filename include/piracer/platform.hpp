#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace piracer {

    // Platform types
    enum class Platform {
        UNKNOWN,
        X86_64,
        ARM64,
        ARM32,
        RISCV64,
        RISCV32,
        POWERPC64,
        POWERPC32
    };

    // Architecture features
    enum class ArchitectureFeature {
        // SIMD features
        SSE2,
        SSE3,
        SSE4_1,
        SSE4_2,
        AVX,
        AVX2,
        AVX512F,
        AVX512BW,
        AVX512DQ,
        AVX512VL,
        
        // ARM features
        NEON,
        SVE,
        SVE2,
        FP16,
        
        // RISC-V features
        RVV,  // Vector extension
        
        // General features
        HARDWARE_MULTIPLY,
        HARDWARE_DIVIDE,
        FLOATING_POINT,
        VECTOR_OPERATIONS
    };

    // Platform information
    struct PlatformInfo {
        Platform platform;
        std::string name;
        std::string vendor;
        std::string model;
        std::string architecture;
        int word_size;  // 32 or 64
        std::vector<ArchitectureFeature> features;
        size_t cache_line_size;
        size_t l1_cache_size;
        size_t l2_cache_size;
        size_t l3_cache_size;
        int num_cores;
        int num_threads;
        
        PlatformInfo() : platform(Platform::UNKNOWN), word_size(64), 
                        cache_line_size(64), l1_cache_size(0), l2_cache_size(0), 
                        l3_cache_size(0), num_cores(1), num_threads(1) {}
    };

    // Platform detection and management
    class PlatformManager {
    public:
        // Get current platform information
        static PlatformInfo get_current_platform();
        
        // Detect platform automatically
        static Platform detect_platform();
        
        // Get platform-specific optimizations
        static std::vector<std::string> get_available_optimizations();
        
        // Check if feature is available
        static bool has_feature(ArchitectureFeature feature);
        
        // Get optimal configuration for platform
        struct PlatformConfig {
            bool use_simd = true;
            bool use_vectorization = true;
            int optimal_threads = 1;
            size_t optimal_chunk_size = 1000;
            std::string preferred_algorithm = "auto";
        };
        
        static PlatformConfig get_optimal_config(size_t digits);
        
        // Platform-specific compilation flags
        static std::vector<std::string> get_compilation_flags();
        
        // Platform-specific runtime optimizations
        static void apply_platform_optimizations();
        
        // Get platform comparison
        static std::map<Platform, PlatformInfo> get_platform_comparison();
        
    private:
        static PlatformInfo current_platform_;
        static bool platform_detected_;
        
        // Platform detection methods
        static Platform detect_x86_64();
        static Platform detect_arm();
        static Platform detect_riscv();
        static Platform detect_powerpc();
        
        // Feature detection methods
        static void detect_x86_features(PlatformInfo& info);
        static void detect_arm_features(PlatformInfo& info);
        static void detect_riscv_features(PlatformInfo& info);
        static void detect_powerpc_features(PlatformInfo& info);
        
        // Cache detection
        static void detect_cache_info(PlatformInfo& info);
    };

    // Cross-platform compilation support
    class CrossPlatformCompiler {
    public:
        // Compile for specific platform
        static bool compile_for_platform(Platform target_platform, 
                                       const std::string& source_dir,
                                       const std::string& build_dir);
        
        // Get cross-compilation toolchain
        static std::string get_toolchain_path(Platform target_platform);
        
        // Validate cross-compilation setup
        static bool validate_cross_compilation(Platform target_platform);
        
        // Get platform-specific build flags
        static std::vector<std::string> get_build_flags(Platform target_platform);
        
    private:
        static std::map<Platform, std::string> toolchain_paths_;
        static std::map<Platform, std::vector<std::string>> build_flags_;
    };

    // Platform-specific optimizations
    class PlatformOptimizer {
    public:
        // Apply platform-specific optimizations
        static void optimize_for_platform(Platform platform);
        
        // Get optimization recommendations
        struct OptimizationRecommendation {
            std::string optimization_name;
            std::string description;
            double expected_improvement;
            bool requires_recompilation;
        };
        
        static std::vector<OptimizationRecommendation> get_recommendations(Platform platform);
        
        // Apply SIMD optimizations
        static void apply_simd_optimizations(Platform platform);
        
        // Apply memory optimizations
        static void apply_memory_optimizations(Platform platform);
        
        // Apply threading optimizations
        static void apply_threading_optimizations(Platform platform);
        
    private:
        static std::map<Platform, std::vector<std::string>> optimization_flags_;
    };

    // Platform benchmarking
    class PlatformBenchmarker {
    public:
        // Benchmark platform performance
        struct PlatformBenchmark {
            Platform platform;
            double performance_ns_per_digit;
            size_t memory_usage_mb;
            double compilation_time_seconds;
            std::string compiler_version;
            std::vector<std::string> applied_optimizations;
        };
        
        static PlatformBenchmark benchmark_platform(Platform platform, 
                                                  size_t digits = 10000);
        
        // Compare platforms
        static std::map<Platform, PlatformBenchmark> benchmark_all_platforms(
            size_t digits = 10000);
        
        // Generate platform comparison report
        static std::string generate_platform_report(
            const std::map<Platform, PlatformBenchmark>& benchmarks);
        
        // Export benchmarks to CSV
        static bool export_benchmarks_to_csv(
            const std::string& filename,
            const std::map<Platform, PlatformBenchmark>& benchmarks);
        
    private:
        static double measure_compilation_time(Platform platform);
        static std::string get_compiler_version(Platform platform);
    };

    // Platform-specific constants
    namespace platform_constants {
        // Cache line sizes
        constexpr size_t X86_64_CACHE_LINE = 64;
        constexpr size_t ARM64_CACHE_LINE = 64;
        constexpr size_t RISCV64_CACHE_LINE = 64;
        constexpr size_t POWERPC64_CACHE_LINE = 128;
        
        // Optimal chunk sizes for different platforms
        constexpr size_t X86_64_OPTIMAL_CHUNK = 1000;
        constexpr size_t ARM64_OPTIMAL_CHUNK = 800;
        constexpr size_t RISCV64_OPTIMAL_CHUNK = 600;
        constexpr size_t POWERPC64_OPTIMAL_CHUNK = 1200;
        
        // Threading recommendations
        constexpr int X86_64_OPTIMAL_THREADS = 8;
        constexpr int ARM64_OPTIMAL_THREADS = 4;
        constexpr int RISCV64_OPTIMAL_THREADS = 2;
        constexpr int POWERPC64_OPTIMAL_THREADS = 16;
    }

} // namespace piracer 