#pragma once
#include <string>
#include <fstream>

namespace piracer {
    // Checkpoint system for resuming long computations
    // This is a skeleton interface for future implementation
    
    struct CheckpointData {
        std::size_t digits;
        int base;
        int num_threads;
        std::size_t completed_terms;
        std::size_t total_terms;
        std::string partial_result;
        
        CheckpointData() = default;
        CheckpointData(std::size_t d, int b, int t, std::size_t c, std::size_t tot, const std::string& res)
            : digits(d), base(b), num_threads(t), completed_terms(c), total_terms(tot), partial_result(res) {}
    };
    
    // Save checkpoint to file
    bool save_checkpoint(const std::string& filename, const CheckpointData& data);
    
    // Load checkpoint from file
    bool load_checkpoint(const std::string& filename, CheckpointData& data);
    
    // Check if checkpoint file exists and is valid
    bool is_valid_checkpoint(const std::string& filename);
    
    // Get checkpoint info without loading full data
    bool get_checkpoint_info(const std::string& filename, std::size_t& digits, int& base, int& threads);
} // namespace piracer 