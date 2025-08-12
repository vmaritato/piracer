#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <functional>
#include "piracer/algorithm_factory.hpp"
#include "piracer/platform.hpp"

namespace piracer {

    // Competition types
    enum class CompetitionType {
        Y_CRUNCHER,      // Primary target
        MINI_PI,         // Already beaten
        CUSTOM_ENGINE,   // User-defined competitor
        BENCHMARK_SUITE  // Standard benchmarks
    };

    // Competition configuration
    struct CompetitionConfig {
        CompetitionType type = CompetitionType::Y_CRUNCHER;
        std::string competitor_path = "";
        std::string competitor_version = "";
        std::vector<size_t> test_digits = {1000, 10000, 100000, 1000000};
        int iterations_per_test = 3;
        bool enable_validation = true;
        bool enable_profiling = true;
        std::string output_format = "detailed";
        
        // Validation settings
        struct ValidationConfig {
            bool check_digit_accuracy = true;
            bool check_performance_consistency = true;
            bool check_memory_usage = true;
            double max_performance_variance_percent = 5.0;
            size_t max_memory_variance_mb = 50;
        } validation;
        
        // Profiling settings
        struct ProfilingConfig {
            bool enable_cpu_profiling = true;
            bool enable_memory_profiling = true;
            bool enable_cache_profiling = true;
            bool enable_gpu_profiling = false;
            int profiling_interval_ms = 100;
        } profiling;
    };

    // Competition result
    struct CompetitionResult {
        CompetitionType type;
        std::string competitor_name;
        std::string competitor_version;
        std::chrono::system_clock::time_point timestamp;
        
        // Performance comparison
        struct PerformanceComparison {
            size_t digits;
            double piracer_time_ms;
            double competitor_time_ms;
            double speedup_factor;
            double piracer_ns_per_digit;
            double competitor_ns_per_digit;
            bool piracer_wins;
        };
        
        std::vector<PerformanceComparison> performance_results;
        
        // Memory comparison
        struct MemoryComparison {
            size_t digits;
            size_t piracer_memory_mb;
            size_t competitor_memory_mb;
            double memory_ratio;
            bool piracer_more_efficient;
        };
        
        std::vector<MemoryComparison> memory_results;
        
        // Overall results
        double average_speedup;
        double best_speedup;
        double worst_speedup;
        bool piracer_overall_winner;
        std::string summary;
        
        CompetitionResult() : type(CompetitionType::Y_CRUNCHER), 
                             average_speedup(0.0), best_speedup(0.0), 
                             worst_speedup(0.0), piracer_overall_winner(false) {}
    };

    // y-cruncher specific competitor
    class YCruncherCompetitor {
    public:
        explicit YCruncherCompetitor(const std::string& ycruncher_path = "");
        ~YCruncherCompetitor() = default;
        
        // Check if y-cruncher is available
        bool is_available() const { return available_; }
        
        // Get y-cruncher version
        std::string get_version() const { return version_; }
        
        // Get y-cruncher capabilities
        struct YCruncherCapabilities {
            bool supports_pi_computation = true;
            bool supports_hex_output = true;
            bool supports_binary_output = false;
            size_t max_digits = 1000000000;  // 1 billion digits
            std::vector<std::string> supported_constants;
            std::vector<std::string> supported_algorithms;
        };
        
        YCruncherCapabilities get_capabilities() const;
        
        // Run y-cruncher benchmark
        struct YCruncherBenchmark {
            size_t digits;
            double computation_time_ms;
            size_t memory_usage_mb;
            std::string output_hash;
            bool success;
            std::string error_message;
        };
        
        YCruncherBenchmark run_benchmark(size_t digits, 
                                        const std::string& output_format = "decimal");
        
        // Validate y-cruncher output
        bool validate_output(const std::string& output_file, size_t expected_digits);
        
        // Get y-cruncher performance profile
        struct YCruncherProfile {
            std::string algorithm_name;
            std::string optimization_level;
            std::vector<std::string> compiler_flags;
            std::string platform_info;
            std::string cpu_info;
            std::string memory_info;
        };
        
        YCruncherProfile get_performance_profile() const;
        
    private:
        std::string ycruncher_path_;
        std::string version_;
        bool available_ = false;
        YCruncherCapabilities capabilities_;
        YCruncherProfile profile_;
        
        // Helper methods
        bool detect_ycruncher();
        bool parse_ycruncher_output(const std::string& output);
        std::string generate_ycruncher_command(size_t digits, const std::string& format);
    };

    // Competition manager
    class CompetitionManager {
    public:
        explicit CompetitionManager(const CompetitionConfig& config);
        ~CompetitionManager() = default;
        
        // Run competition
        CompetitionResult run_competition();
        
        // Run specific competition type
        CompetitionResult run_ycruncher_competition();
        CompetitionResult run_minipi_competition();
        CompetitionResult run_custom_competition(const std::string& competitor_path);
        CompetitionResult run_benchmark_suite();
        
        // Get competition history
        std::vector<CompetitionResult> get_competition_history() const;
        
        // Export competition results
        bool export_results_to_csv(const std::string& filename, 
                                 const CompetitionResult& result);
        bool export_results_to_json(const std::string& filename, 
                                  const CompetitionResult& result);
        bool export_results_to_html(const std::string& filename, 
                                  const CompetitionResult& result);
        
        // Generate competition report
        std::string generate_competition_report(const CompetitionResult& result);
        
        // Get competition statistics
        struct CompetitionStatistics {
            int total_competitions;
            int piracer_wins;
            int competitor_wins;
            double average_speedup;
            double best_speedup_ever;
            std::chrono::system_clock::time_point last_competition;
            std::map<CompetitionType, int> competitions_by_type;
        };
        
        CompetitionStatistics get_competition_statistics() const;
        
        // Set competition configuration
        void update_config(const CompetitionConfig& new_config);
        
        // Get current configuration
        CompetitionConfig get_current_config() const { return config_; }
        
    private:
        CompetitionConfig config_;
        std::vector<CompetitionResult> competition_history_;
        std::unique_ptr<YCruncherCompetitor> ycruncher_;
        
        // Competition methods
        CompetitionResult run_performance_tests();
        CompetitionResult run_memory_tests();
        CompetitionResult run_validation_tests();
        
        // Result analysis
        void analyze_performance_results(CompetitionResult& result);
        void analyze_memory_results(CompetitionResult& result);
        void generate_summary(CompetitionResult& result);
        
        // Validation methods
        bool validate_competition_results(const CompetitionResult& result);
        bool check_performance_consistency(const CompetitionResult& result);
        bool check_memory_consistency(const CompetitionResult& result);
    };

    // Competition analyzer
    class CompetitionAnalyzer {
    public:
        CompetitionAnalyzer() = default;
        ~CompetitionAnalyzer() = default;
        
        // Analyze competition results
        struct CompetitionAnalysis {
            std::string competitor_name;
            std::string analysis_date;
            
            // Performance analysis
            struct PerformanceAnalysis {
                double overall_speedup;
                std::map<size_t, double> speedup_by_digits;
                std::string best_performance_range;
                std::string worst_performance_range;
                double performance_consistency_score;
                std::vector<std::string> performance_insights;
            } performance;
            
            // Memory analysis
            struct MemoryAnalysis {
                double overall_memory_efficiency;
                std::map<size_t, double> memory_efficiency_by_digits;
                std::string memory_usage_pattern;
                double memory_scalability_score;
                std::vector<std::string> memory_insights;
            } memory;
            
            // Strategic analysis
            struct StrategicAnalysis {
                std::string competitive_advantage;
                std::vector<std::string> improvement_areas;
                std::vector<std::string> strengths;
                std::vector<std::string> weaknesses;
                std::string recommendation;
            } strategy;
        };
        
        CompetitionAnalysis analyze_competition(const CompetitionResult& result);
        
        // Compare multiple competitions
        struct CompetitionComparison {
            std::vector<std::string> competitor_names;
            std::map<std::string, double> average_speedups;
            std::map<std::string, double> memory_efficiencies;
            std::string overall_winner;
            std::vector<std::string> key_insights;
        };
        
        CompetitionComparison compare_competitions(
            const std::vector<CompetitionResult>& results);
        
        // Generate improvement recommendations
        struct ImprovementRecommendation {
            std::string area;
            std::string description;
            double expected_improvement;
            int priority;  // 1-5, 5 being highest
            std::vector<std::string> implementation_steps;
        };
        
        std::vector<ImprovementRecommendation> get_improvement_recommendations(
            const CompetitionResult& result);
        
        // Export analysis
        bool export_analysis_to_csv(const std::string& filename, 
                                  const CompetitionAnalysis& analysis);
        bool export_analysis_to_json(const std::string& filename, 
                                   const CompetitionAnalysis& analysis);
        
    private:
        // Analysis methods
        void analyze_performance_patterns(CompetitionAnalysis::PerformanceAnalysis& analysis,
                                        const CompetitionResult& result);
        void analyze_memory_patterns(CompetitionAnalysis::MemoryAnalysis& analysis,
                                   const CompetitionResult& result);
        void generate_strategic_insights(CompetitionAnalysis::StrategicAnalysis& analysis,
                                       const CompetitionResult& result);
    };

    // Competition visualization
    class CompetitionVisualizer {
    public:
        CompetitionVisualizer() = default;
        ~CompetitionVisualizer() = default;
        
        // Create performance comparison charts
        bool create_performance_chart(const CompetitionResult& result, 
                                    const std::string& output_file);
        
        // Create memory comparison charts
        bool create_memory_chart(const CompetitionResult& result, 
                               const std::string& output_file);
        
        // Create speedup analysis charts
        bool create_speedup_chart(const CompetitionResult& result, 
                                const std::string& output_file);
        
        // Create comprehensive dashboard
        bool create_competition_dashboard(const CompetitionResult& result, 
                                        const std::string& output_file);
        
        // Create historical trend charts
        bool create_trend_chart(const std::vector<CompetitionResult>& history, 
                              const std::string& output_file);
        
    private:
        // Chart generation methods
        void generate_performance_data(const CompetitionResult& result);
        void generate_memory_data(const CompetitionResult& result);
        void generate_speedup_data(const CompetitionResult& result);
    };

} // namespace piracer 