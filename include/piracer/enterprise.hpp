#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <functional>

namespace piracer {

    // Enterprise feature levels
    enum class EnterpriseLevel {
        COMMUNITY,      // Open source, basic features
        PROFESSIONAL,   // Enhanced features, support
        ENTERPRISE,     // Full features, SLA, support
        CUSTOM          // Custom requirements
    };

    // Service Level Agreement (SLA) metrics
    struct SLAMetrics {
        double uptime_percentage = 99.9;  // 99.9% uptime
        int max_response_time_ms = 1000;  // 1 second max response
        int max_error_rate_percent = 0.1; // 0.1% max error rate
        int support_response_hours = 4;   // 4 hour support response
        bool includes_24x7_support = false;
        
        SLAMetrics() = default;
        SLAMetrics(double uptime, int response_time, int error_rate, 
                   int support_response, bool support_24x7)
            : uptime_percentage(uptime), max_response_time_ms(response_time),
              max_error_rate_percent(error_rate), support_response_hours(support_response),
              includes_24x7_support(support_24x7) {}
    };

    // Enterprise configuration
    struct EnterpriseConfig {
        EnterpriseLevel level = EnterpriseLevel::COMMUNITY;
        SLAMetrics sla_requirements;
        bool enable_monitoring = true;
        bool enable_logging = true;
        bool enable_metrics = true;
        bool enable_alerting = false;
        std::string license_key = "";
        std::string support_contact = "";
        std::string deployment_environment = "production";
        
        // Monitoring configuration
        struct MonitoringConfig {
            bool enable_performance_monitoring = true;
            bool enable_resource_monitoring = true;
            bool enable_error_monitoring = true;
            int monitoring_interval_seconds = 60;
            bool enable_anomaly_detection = false;
            std::vector<std::string> alert_recipients;
        } monitoring;
        
        // Logging configuration
        struct LoggingConfig {
            std::string log_level = "INFO";
            std::string log_format = "JSON";
            std::string log_destination = "file";
            std::string log_file_path = "/var/log/piracer/";
            bool enable_log_rotation = true;
            size_t max_log_file_size_mb = 100;
            int max_log_files = 10;
        } logging;
        
        // Security configuration
        struct SecurityConfig {
            bool enable_encryption = false;
            bool enable_authentication = false;
            bool enable_authorization = false;
            std::string encryption_algorithm = "AES-256";
            std::vector<std::string> allowed_networks;
        } security;
    };

    // Enterprise monitoring system
    class EnterpriseMonitor {
    public:
        explicit EnterpriseMonitor(const EnterpriseConfig& config);
        ~EnterpriseMonitor() = default;
        
        // Start monitoring
        void start_monitoring();
        
        // Stop monitoring
        void stop_monitoring();
        
        // Get monitoring status
        bool is_monitoring_active() const { return monitoring_active_; }
        
        // Performance monitoring
        struct PerformanceMetrics {
            double cpu_usage_percent;
            double memory_usage_percent;
            double disk_io_mbps;
            double network_io_mbps;
            int active_connections;
            double response_time_ms;
            int requests_per_second;
            std::chrono::system_clock::time_point timestamp;
        };
        
        PerformanceMetrics get_performance_metrics() const;
        
        // Resource monitoring
        struct ResourceMetrics {
            size_t total_memory_mb;
            size_t used_memory_mb;
            size_t total_disk_gb;
            size_t used_disk_gb;
            int cpu_cores;
            double cpu_frequency_ghz;
            std::string os_version;
            std::string kernel_version;
        };
        
        ResourceMetrics get_resource_metrics() const;
        
        // Error monitoring
        struct ErrorMetrics {
            int total_errors;
            int errors_last_hour;
            std::map<std::string, int> error_types;
            std::vector<std::string> recent_errors;
            double error_rate_percent;
        };
        
        ErrorMetrics get_error_metrics() const;
        
        // SLA compliance checking
        bool check_sla_compliance() const;
        
        // Generate monitoring report
        std::string generate_monitoring_report() const;
        
        // Export metrics to various formats
        bool export_metrics_to_csv(const std::string& filename) const;
        bool export_metrics_to_json(const std::string& filename) const;
        bool export_metrics_to_prometheus(const std::string& filename) const;
        
    private:
        EnterpriseConfig config_;
        bool monitoring_active_ = false;
        std::chrono::system_clock::time_point start_time_;
        
        // Monitoring data storage
        std::vector<PerformanceMetrics> performance_history_;
        std::vector<ResourceMetrics> resource_history_;
        std::vector<ErrorMetrics> error_history_;
        
        // Monitoring methods
        void collect_performance_metrics();
        void collect_resource_metrics();
        void collect_error_metrics();
        void check_alerts();
        
        // Alert system
        void send_alert(const std::string& message, const std::string& severity);
    };

    // Enterprise logging system
    class EnterpriseLogger {
    public:
        explicit EnterpriseLogger(const EnterpriseConfig& config);
        ~EnterpriseLogger() = default;
        
        // Log levels
        enum class LogLevel {
            TRACE,
            DEBUG,
            INFO,
            WARNING,
            ERROR,
            CRITICAL
        };
        
        // Log messages
        void log(LogLevel level, const std::string& message, 
                const std::string& component = "", 
                const std::map<std::string, std::string>& context = {});
        
        void trace(const std::string& message, const std::string& component = "");
        void debug(const std::string& message, const std::string& component = "");
        void info(const std::string& message, const std::string& component = "");
        void warning(const std::string& message, const std::string& component = "");
        void error(const std::string& message, const std::string& component = "");
        void critical(const std::string& message, const std::string& component = "");
        
        // Structured logging
        void log_structured(LogLevel level, const std::string& event_type,
                          const std::map<std::string, std::string>& fields);
        
        // Log rotation
        void rotate_logs();
        
        // Get log statistics
        struct LogStatistics {
            int total_logs;
            std::map<LogLevel, int> logs_by_level;
            std::map<std::string, int> logs_by_component;
            std::chrono::system_clock::time_point last_log_time;
            size_t current_log_file_size_mb;
        };
        
        LogStatistics get_log_statistics() const;
        
        // Search logs
        std::vector<std::string> search_logs(const std::string& query, 
                                           LogLevel min_level = LogLevel::TRACE,
                                           const std::chrono::system_clock::time_point& since = {});
        
    private:
        EnterpriseConfig config_;
        std::ofstream log_file_;
        std::string current_log_file_;
        
        // Log formatting
        std::string format_log_message(LogLevel level, const std::string& message,
                                     const std::string& component,
                                     const std::map<std::string, std::string>& context);
        
        // Log file management
        void open_log_file();
        void close_log_file();
        std::string get_timestamp_string() const;
    };

    // Enterprise security system
    class EnterpriseSecurity {
    public:
        explicit EnterpriseSecurity(const EnterpriseConfig& config);
        ~EnterpriseSecurity() = default;
        
        // Authentication
        struct User {
            std::string username;
            std::string email;
            std::vector<std::string> roles;
            std::chrono::system_clock::time_point last_login;
            bool is_active;
        };
        
        bool authenticate_user(const std::string& username, const std::string& password);
        bool validate_token(const std::string& token);
        std::string generate_token(const std::string& username);
        
        // Authorization
        bool check_permission(const std::string& username, const std::string& resource, 
                            const std::string& action);
        std::vector<std::string> get_user_permissions(const std::string& username);
        
        // Encryption
        std::string encrypt_data(const std::string& data, const std::string& key);
        std::string decrypt_data(const std::string& encrypted_data, const std::string& key);
        std::string generate_encryption_key();
        
        // Network security
        bool is_ip_allowed(const std::string& ip_address);
        bool validate_network_request(const std::string& source_ip, const std::string& endpoint);
        
        // Security audit
        struct SecurityEvent {
            std::string event_type;
            std::string username;
            std::string resource;
            std::string action;
            std::string result;
            std::string ip_address;
            std::chrono::system_clock::time_point timestamp;
        };
        
        void log_security_event(const SecurityEvent& event);
        std::vector<SecurityEvent> get_security_events(const std::chrono::system_clock::time_point& since = {});
        
    private:
        EnterpriseConfig config_;
        std::map<std::string, User> users_;
        std::map<std::string, std::vector<std::string>> permissions_;
        std::vector<SecurityEvent> security_events_;
        
        // Security methods
        std::string hash_password(const std::string& password);
        bool verify_password(const std::string& password, const std::string& hash);
        std::string generate_random_string(size_t length);
    };

    // Enterprise license manager
    class EnterpriseLicenseManager {
    public:
        explicit EnterpriseLicenseManager(const EnterpriseConfig& config);
        ~EnterpriseLicenseManager() = default;
        
        // License validation
        bool validate_license(const std::string& license_key);
        bool is_license_valid() const { return license_valid_; }
        
        // License information
        struct LicenseInfo {
            std::string license_key;
            std::string customer_name;
            std::string customer_email;
            EnterpriseLevel level;
            std::chrono::system_clock::time_point issue_date;
            std::chrono::system_clock::time_point expiry_date;
            std::vector<std::string> features;
            int max_users;
            int max_instances;
        };
        
        LicenseInfo get_license_info() const;
        
        // Feature access control
        bool has_feature_access(const std::string& feature_name);
        std::vector<std::string> get_available_features();
        
        // License renewal
        bool renew_license(const std::string& new_license_key);
        
        // License upgrade
        bool upgrade_license(EnterpriseLevel new_level);
        
    private:
        EnterpriseConfig config_;
        bool license_valid_ = false;
        LicenseInfo current_license_;
        
        // License validation methods
        bool validate_license_format(const std::string& license_key);
        bool validate_license_signature(const std::string& license_key);
        bool validate_license_expiry(const std::string& license_key);
        bool validate_license_features(const std::string& license_key);
    };

    // Enterprise support system
    class EnterpriseSupport {
    public:
        explicit EnterpriseSupport(const EnterpriseConfig& config);
        ~EnterpriseSupport() = default;
        
        // Support ticket system
        struct SupportTicket {
            std::string ticket_id;
            std::string customer_name;
            std::string customer_email;
            std::string subject;
            std::string description;
            std::string priority;  // LOW, MEDIUM, HIGH, CRITICAL
            std::string status;    // OPEN, IN_PROGRESS, RESOLVED, CLOSED
            std::chrono::system_clock::time_point created_at;
            std::chrono::system_clock::time_point updated_at;
            std::vector<std::string> attachments;
        };
        
        std::string create_support_ticket(const std::string& customer_name,
                                        const std::string& customer_email,
                                        const std::string& subject,
                                        const std::string& description,
                                        const std::string& priority = "MEDIUM");
        
        bool update_ticket_status(const std::string& ticket_id, const std::string& status);
        bool add_ticket_comment(const std::string& ticket_id, const std::string& comment);
        SupportTicket get_ticket_info(const std::string& ticket_id);
        
        // Support response time tracking
        struct SupportMetrics {
            int total_tickets;
            int open_tickets;
            int resolved_tickets;
            double average_response_time_hours;
            double average_resolution_time_hours;
            std::map<std::string, int> tickets_by_priority;
            std::map<std::string, int> tickets_by_status;
        };
        
        SupportMetrics get_support_metrics() const;
        
        // SLA compliance checking
        bool check_support_sla_compliance() const;
        
        // Support documentation
        std::vector<std::string> get_support_documents();
        std::string get_support_document(const std::string& document_name);
        
    private:
        EnterpriseConfig config_;
        std::map<std::string, SupportTicket> tickets_;
        std::vector<std::string> support_documents_;
        
        // Support methods
        std::string generate_ticket_id();
        void update_ticket_metrics();
        bool validate_customer_access(const std::string& customer_email);
    };

} // namespace piracer 