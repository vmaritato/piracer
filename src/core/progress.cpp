#include "piracer/progress.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace piracer {
    
    void ProgressTracker::set_total(std::size_t total) {
        total_ = total;
        current_ = 0;
        progress_percentage_ = 0.0;
        start_time_ = std::chrono::system_clock::now();
        last_update_time_ = start_time_;
        
        report_event(ProgressEventType::STARTED, "Computation started", 
                    "Total terms: " + std::to_string(total));
    }
    
    void ProgressTracker::update(std::size_t current, const std::string& message) {
        current_ = current;
        progress_percentage_ = total_ > 0 ? (100.0 * current / total_) : 0.0;
        
        auto now = std::chrono::system_clock::now();
        last_update_time_ = now;
        
        update_performance_metrics();
        
        if (!message.empty()) {
            report_event(ProgressEventType::TERM_COMPLETED, message);
        }
    }
    
    void ProgressTracker::increment(std::size_t amount, const std::string& message) {
        update(current_ + amount, message);
    }
    
    void ProgressTracker::report_event(ProgressEventType type, const std::string& message, 
                                      const std::string& additional_data) {
        ProgressEvent event(type, message, current_, total_, additional_data);
        notify_progress_callbacks(event);
        
        // Also log the event
        std::string event_type_str;
        switch (type) {
            case ProgressEventType::STARTED: event_type_str = "STARTED"; break;
            case ProgressEventType::TERM_COMPLETED: event_type_str = "TERM_COMPLETED"; break;
            case ProgressEventType::CHECKPOINT_SAVED: event_type_str = "CHECKPOINT_SAVED"; break;
            case ProgressEventType::CHECKPOINT_LOADED: event_type_str = "CHECKPOINT_LOADED"; break;
            case ProgressEventType::RESUME_DETECTED: event_type_str = "RESUME_DETECTED"; break;
            case ProgressEventType::ALGORITHM_CHANGED: event_type_str = "ALGORITHM_CHANGED"; break;
            case ProgressEventType::PERFORMANCE_UPDATE: event_type_str = "PERFORMANCE_UPDATE"; break;
            case ProgressEventType::ERROR_OCCURRED: event_type_str = "ERROR_OCCURRED"; break;
            case ProgressEventType::COMPLETED: event_type_str = "COMPLETED"; break;
        }
        
        log_info("[" + event_type_str + "] " + message);
    }
    
    void ProgressTracker::add_progress_callback(ProgressCallback callback) {
        progress_callbacks_.push_back(std::move(callback));
    }
    
    void ProgressTracker::add_log_callback(LogCallback callback) {
        log_callbacks_.push_back(std::move(callback));
    }
    
    void ProgressTracker::remove_progress_callback(size_t index) {
        if (index < progress_callbacks_.size()) {
            progress_callbacks_.erase(progress_callbacks_.begin() + index);
        }
    }
    
    void ProgressTracker::remove_log_callback(size_t index) {
        if (index < log_callbacks_.size()) {
            log_callbacks_.erase(log_callbacks_.begin() + index);
        }
    }
    
    double ProgressTracker::get_rate_per_second() const {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now() - start_time_).count();
        
        if (elapsed == 0) return 0.0;
        return static_cast<double>(current_) / elapsed;
    }
    
    std::chrono::seconds ProgressTracker::get_estimated_time_remaining() const {
        if (current_ == 0 || total_ == 0) return std::chrono::seconds::max();
        
        double rate = get_rate_per_second();
        if (rate <= 0) return std::chrono::seconds::max();
        
        std::size_t remaining = total_ - current_;
        double seconds_remaining = remaining / rate;
        
        return std::chrono::seconds(static_cast<long>(seconds_remaining));
    }
    
    std::chrono::seconds ProgressTracker::get_elapsed_time() const {
        return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now() - start_time_);
    }
    
    void ProgressTracker::log_info(const std::string& message) {
        notify_log_callbacks("INFO", message);
    }
    
    void ProgressTracker::log_warning(const std::string& message) {
        notify_log_callbacks("WARNING", message);
    }
    
    void ProgressTracker::log_error(const std::string& message) {
        notify_log_callbacks("ERROR", message);
    }
    
    void ProgressTracker::log_performance(const std::string& metric, double value, const std::string& unit) {
        std::stringstream ss;
        ss << metric << ": " << std::fixed << std::setprecision(3) << value;
        if (!unit.empty()) {
            ss << " " << unit;
        }
        notify_log_callbacks("PERFORMANCE", ss.str());
    }
    
    void ProgressTracker::report_checkpoint_saved(const std::string& filename) {
        report_event(ProgressEventType::CHECKPOINT_SAVED, "Checkpoint saved", filename);
    }
    
    void ProgressTracker::report_checkpoint_loaded(const std::string& filename) {
        report_event(ProgressEventType::CHECKPOINT_LOADED, "Checkpoint loaded", filename);
    }
    
    void ProgressTracker::report_resume_detected(const std::string& filename) {
        report_event(ProgressEventType::RESUME_DETECTED, "Resume detected", filename);
    }
    
    void ProgressTracker::set_algorithm(const std::string& name) {
        if (algorithm_name_ != name) {
            algorithm_name_ = name;
            report_event(ProgressEventType::ALGORITHM_CHANGED, "Algorithm changed", name);
        }
    }
    
    void ProgressTracker::reset() {
        total_ = 0;
        current_ = 0;
        progress_percentage_ = 0.0;
        start_time_ = std::chrono::system_clock::now();
        last_update_time_ = start_time_;
        algorithm_name_ = "unknown";
    }
    
    void ProgressTracker::notify_progress_callbacks(const ProgressEvent& event) {
        for (const auto& callback : progress_callbacks_) {
            try {
                callback(event);
            } catch (...) {
                // Ignore callback errors to prevent crashes
            }
        }
    }
    
    void ProgressTracker::notify_log_callbacks(const std::string& level, const std::string& message) {
        for (const auto& callback : log_callbacks_) {
            try {
                callback(level, message);
            } catch (...) {
                // Ignore callback errors to prevent crashes
            }
        }
    }
    
    void ProgressTracker::update_performance_metrics() {
        // Report performance updates periodically
        static int update_counter = 0;
        if (++update_counter % 100 == 0) {  // Every 100 updates
            double rate = get_rate_per_second();
            log_performance("Terms per second", rate, "terms/s");
            
            auto eta = get_estimated_time_remaining();
            if (eta != std::chrono::seconds::max()) {
                log_performance("ETA", eta.count(), "seconds");
            }
        }
    }
    
} // namespace piracer 