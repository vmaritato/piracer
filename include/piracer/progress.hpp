#pragma once
#include <cstddef>
#include <string>
#include <chrono>
#include <functional>
#include <vector>
#include <memory>

namespace piracer {
    
    // Progress event types
    enum class ProgressEventType {
        STARTED,
        TERM_COMPLETED,
        CHECKPOINT_SAVED,
        CHECKPOINT_LOADED,
        RESUME_DETECTED,
        ALGORITHM_CHANGED,
        PERFORMANCE_UPDATE,
        ERROR_OCCURRED,
        COMPLETED
    };
    
    // Progress event structure
    struct ProgressEvent {
        ProgressEventType type;
        std::string message;
        std::chrono::system_clock::time_point timestamp;
        std::size_t current_value;
        std::size_t total_value;
        double progress_percentage;
        std::string additional_data;
        
        ProgressEvent(ProgressEventType t, const std::string& msg, 
                     std::size_t current, std::size_t total, 
                     const std::string& data = "")
            : type(t), message(msg), timestamp(std::chrono::system_clock::now()),
              current_value(current), total_value(total), 
              progress_percentage(total > 0 ? (100.0 * current / total) : 0.0),
              additional_data(data) {}
    };
    
    // Progress callback types
    using ProgressCallback = std::function<void(const ProgressEvent&)>;
    using LogCallback = std::function<void(const std::string&, const std::string&)>;
    
    // Enhanced progress reporting for long-running computations
    class ProgressTracker {
    public:
        ProgressTracker() = default;
        ~ProgressTracker() = default;
        
        // Basic progress tracking
        void set_total(std::size_t total);
        void update(std::size_t current, const std::string& message = "");
        void increment(std::size_t amount = 1, const std::string& message = "");
        
        // Event reporting
        void report_event(ProgressEventType type, const std::string& message, 
                         const std::string& additional_data = "");
        
        // Callback management
        void add_progress_callback(ProgressCallback callback);
        void add_log_callback(LogCallback callback);
        void remove_progress_callback(size_t index);
        void remove_log_callback(size_t index);
        
        // Progress information
        std::size_t get_total() const { return total_; }
        std::size_t get_current() const { return current_; }
        double get_percentage() const { return progress_percentage_; }
        std::chrono::system_clock::time_point get_start_time() const { return start_time_; }
        
        // Performance metrics
        double get_rate_per_second() const;
        std::chrono::seconds get_estimated_time_remaining() const;
        std::chrono::seconds get_elapsed_time() const;
        
        // Logging
        void log_info(const std::string& message);
        void log_warning(const std::string& message);
        void log_error(const std::string& message);
        void log_performance(const std::string& metric, double value, const std::string& unit = "");
        
        // Checkpoint integration
        void report_checkpoint_saved(const std::string& filename);
        void report_checkpoint_loaded(const std::string& filename);
        void report_resume_detected(const std::string& filename);
        
        // Algorithm information
        void set_algorithm(const std::string& name);
        std::string get_algorithm() const { return algorithm_name_; }
        
        // Reset progress
        void reset();
        
    private:
        std::size_t total_ = 0;
        std::size_t current_ = 0;
        double progress_percentage_ = 0.0;
        std::chrono::system_clock::time_point start_time_;
        std::chrono::system_clock::time_point last_update_time_;
        std::string algorithm_name_ = "unknown";
        
        std::vector<ProgressCallback> progress_callbacks_;
        std::vector<LogCallback> log_callbacks_;
        
        void notify_progress_callbacks(const ProgressEvent& event);
        void notify_log_callbacks(const std::string& level, const std::string& message);
        void update_performance_metrics();
    };
    
    // Legacy compatibility
    struct Progress {
        std::size_t total = 0;
        std::size_t done  = 0;
        void (*tick)(std::size_t done, std::size_t total, void* user) = nullptr;
        void* user = nullptr;
        
        // Constructor for easy conversion
        Progress() = default;
        Progress(std::size_t t, std::size_t d) : total(t), done(d) {}
        
        // Conversion from ProgressTracker
        Progress(const ProgressTracker& tracker) 
            : total(tracker.get_total()), done(tracker.get_current()) {}
    };
    
} // namespace piracer
