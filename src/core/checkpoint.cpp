#include "piracer/checkpoint.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <cstring>

namespace piracer {
    
    // Simple JSON serializer for checkpoint data
    namespace {
        std::string escape_json_string(const std::string& str) {
            std::string result;
            result.reserve(str.length());
            
            for (char c : str) {
                switch (c) {
                    case '"': result += "\\\""; break;
                    case '\\': result += "\\\\"; break;
                    case '\b': result += "\\b"; break;
                    case '\f': result += "\\f"; break;
                    case '\n': result += "\\n"; break;
                    case '\r': result += "\\r"; break;
                    case '\t': result += "\\t"; break;
                    default: result += c; break;
                }
            }
            return result;
        }
        
        std::string timestamp_to_string(const std::chrono::system_clock::time_point& tp) {
            auto time_t = std::chrono::system_clock::to_time_t(tp);
            std::stringstream ss;
            ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ");
            return ss.str();
        }
        
        std::chrono::system_clock::time_point string_to_timestamp(const std::string& str) {
            std::tm tm = {};
            std::stringstream ss(str);
            ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
            return std::chrono::system_clock::from_time_t(std::mktime(&tm));
        }
        
        std::string simple_hash(const std::string& data) {
            // Simple hash for integrity checking
            std::hash<std::string> hasher;
            std::stringstream ss;
            ss << std::hex << hasher(data);
            return ss.str();
        }
    }
    
    std::string generate_checksum(const CheckpointData& data) {
        std::stringstream ss;
        ss << data.digits << "|" << data.base << "|" << data.num_threads 
           << "|" << data.completed_terms << "|" << data.total_terms 
           << "|" << data.partial_result << "|" << data.algorithm_name;
        return simple_hash(ss.str());
    }
    
    bool save_checkpoint(const std::string& filename, const CheckpointData& data) {
        // Generate checksum for integrity
        std::string checksum = generate_checksum(data);
        
        std::ofstream file(filename);
        if (!file) {
            return false;
        }
        
        // JSON format
        file << "{\n";
        file << "  \"version\": \"1.0\",\n";
        file << "  \"digits\": " << data.digits << ",\n";
        file << "  \"base\": " << data.base << ",\n";
        file << "  \"threads\": " << data.num_threads << ",\n";
        file << "  \"completed_terms\": " << data.completed_terms << ",\n";
        file << "  \"total_terms\": " << data.total_terms << ",\n";
        file << "  \"algorithm\": \"" << escape_json_string(data.algorithm_name) << "\",\n";
        file << "  \"timestamp\": \"" << timestamp_to_string(data.timestamp) << "\",\n";
        file << "  \"checksum\": \"" << checksum << "\",\n";
        file << "  \"partial_result\": \"" << escape_json_string(data.partial_result) << "\"\n";
        file << "}\n";
        
        return file.good();
    }
    
    bool load_checkpoint(const std::string& filename, CheckpointData& data) {
        std::ifstream file(filename);
        if (!file) {
            return false;
        }
        
        std::string line;
        std::string content;
        while (std::getline(file, line)) {
            content += line + "\n";
        }
        
        // Simple JSON parsing (for now)
        // In production, use a proper JSON library
        try {
            // Extract values using string operations
            auto extract_value = [&content](const std::string& key) -> std::string {
                size_t pos = content.find("\"" + key + "\":");
                if (pos == std::string::npos) return "";
                
                pos = content.find(":", pos);
                if (pos == std::string::npos) return "";
                
                pos = content.find_first_not_of(" \t", pos + 1);
                if (pos == std::string::npos) return "";
                
                size_t end_pos;
                if (content[pos] == '"') {
                    pos++;
                    end_pos = content.find("\"", pos);
                    if (end_pos == std::string::npos) return "";
                } else {
                    end_pos = content.find_first_of(",\n}", pos);
                    if (end_pos == std::string::npos) end_pos = content.length();
                }
                
                return content.substr(pos, end_pos - pos);
            };
            
            data.digits = std::stoull(extract_value("digits"));
            data.base = std::stoi(extract_value("base"));
            data.num_threads = std::stoi(extract_value("threads"));
            data.completed_terms = std::stoull(extract_value("completed_terms"));
            data.total_terms = std::stoull(extract_value("total_terms"));
            data.algorithm_name = extract_value("algorithm");
            data.timestamp = string_to_timestamp(extract_value("timestamp"));
            data.checksum = extract_value("checksum");
            data.partial_result = extract_value("partial_result");
            
            return true;
        } catch (...) {
            return false;
        }
    }
    
    bool is_valid_checkpoint(const std::string& filename) {
        if (!std::filesystem::exists(filename)) {
            return false;
        }
        
        CheckpointData data;
        if (!load_checkpoint(filename, data)) {
            return false;
        }
        
        // Validate checksum
        std::string expected_checksum = generate_checksum(data);
        return expected_checksum == data.checksum;
    }
    
    bool validate_checkpoint_integrity(const std::string& filename) {
        return is_valid_checkpoint(filename);
    }
    
    std::chrono::hours get_checkpoint_age(const std::string& filename) {
        CheckpointData data;
        if (!load_checkpoint(filename, data)) {
            return std::chrono::hours::max();
        }
        
        auto now = std::chrono::system_clock::now();
        auto duration = now - data.timestamp;
        return std::chrono::duration_cast<std::chrono::hours>(duration);
    }
    
    bool cleanup_old_checkpoints(const std::string& directory, std::chrono::hours max_age) {
        try {
            std::filesystem::path dir_path(directory);
            if (!std::filesystem::exists(dir_path) || !std::filesystem::is_directory(dir_path)) {
                return false;
            }
            
            bool cleaned = false;
            for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
                if (entry.is_regular_file() && entry.path().extension() == ".checkpoint") {
                    auto age = get_checkpoint_age(entry.path().string());
                    if (age > max_age) {
                        std::filesystem::remove(entry.path());
                        cleaned = true;
                    }
                }
            }
            
            return cleaned;
        } catch (...) {
            return false;
        }
    }
    
    bool get_checkpoint_info(const std::string& filename, std::size_t& digits, int& base, int& threads) {
        CheckpointData data;
        if (!load_checkpoint(filename, data)) {
            return false;
        }
        
        digits = data.digits;
        base = data.base;
        threads = data.num_threads;
        return true;
    }
    
} // namespace piracer 