#pragma once
#include <string>
#include <fstream>
#include <chrono>
#include <vector>

namespace piracer {
    
    // Checkpoint system for resuming long computations
    // Supports JSON serialization and advanced validation
    
    struct CheckpointData {
        std::size_t digits;
        int base;
        int num_threads;
        std::size_t completed_terms;
        std::size_t total_terms;
        std::string partial_result;
        std::string algorithm_name;
        std::chrono::system_clock::time_point timestamp;
        std::string checksum;  // For integrity validation
        
        CheckpointData() = default;
        CheckpointData(std::size_t d, int b, int t, std::size_t c, std::size_t tot, 
                      const std::string& res, const std::string& alg = "chudnovsky")
            : digits(d), base(b), num_threads(t), completed_terms(c), total_terms(tot), 
              partial_result(res), algorithm_name(alg), timestamp(std::chrono::system_clock::now()) {}
    };
    
    // Save checkpoint to file with JSON format
    bool save_checkpoint(const std::string& filename, const CheckpointData& data);
    
    // Load checkpoint from file
    bool load_checkpoint(const std::string& filename, CheckpointData& data);
    
    // Check if checkpoint file exists and is valid
    bool is_valid_checkpoint(const std::string& filename);
    
    // Get checkpoint info without loading full data
    bool get_checkpoint_info(const std::string& filename, std::size_t& digits, int& base, int& threads);
    
    // Validate checkpoint integrity
    bool validate_checkpoint_integrity(const std::string& filename);
    
    // Get checkpoint age (how old is the checkpoint)
    std::chrono::hours get_checkpoint_age(const std::string& filename);
    
    // Clean up old checkpoints
    bool cleanup_old_checkpoints(const std::string& directory, std::chrono::hours max_age);
    
    // Generate checksum for data integrity
    std::string generate_checksum(const CheckpointData& data);
    
} // namespace piracer 