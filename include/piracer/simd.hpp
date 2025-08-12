#pragma once
#include <cstdint>
#include <immintrin.h>  // Intel intrinsics

namespace piracer {
    
    // SIMD optimizations for different architectures
    namespace simd {
        
        // Detect CPU features
        struct CPUFeatures {
            bool sse2 = false;
            bool sse3 = false;
            bool sse4_1 = false;
            bool avx = false;
            bool avx2 = false;
            bool avx512 = false;
            bool neon = false;  // ARM
        };
        
                     // Get CPU features
             CPUFeatures get_cpu_features();
             
             // Detect and log SIMD capabilities
             void log_simd_capabilities();
             
             // Check if specific features are available
             bool has_avx512() const;
             bool has_neon() const;
             bool has_sve() const;  // ARM Scalable Vector Extension
        
        // Vectorized operations for different data types
        
        // 32-bit integer operations
        namespace i32 {
            // Add 8 integers at once
            __m256i add_8(__m256i a, __m256i b);
            
            // Multiply 8 integers at once
            __m256i mul_8(__m256i a, __m256i b);
            
            // Load from array
            __m256i load_8(const int32_t* ptr);
            
            // Store to array
            void store_8(int32_t* ptr, __m256i val);
        }
        
        // 64-bit integer operations
        namespace i64 {
            // Add 4 integers at once
            __m256i add_4(__m256i a, __m256i b);
            
            // Multiply 4 integers at once
            __m256i mul_4(__m256i a, __m256i b);
            
            // Load from array
            __m256i load_4(const int64_t* ptr);
            
            // Store to array
            void store_4(int64_t* ptr, __m256i val);
        }
        
        // Floating point operations
        namespace f64 {
            // Add 4 doubles at once
            __m256d add_4(__m256d a, __m256d b);
            
            // Multiply 4 doubles at once
            __m256d mul_4(__m256d a, __m256d b);
            
            // Load from array
            __m256d load_4(const double* ptr);
            
            // Store to array
            void store_4(double* ptr, __m256d val);
        }
        
                     // NTT-specific optimizations
             namespace ntt {
                 // Vectorized butterfly operation
                 void butterfly_4(__m256d& a, __m256d& b, __m256d w);

                 // Vectorized twiddle factor multiplication
                 __m256d twiddle_mul_4(__m256d data, __m256d twiddle);
                 
                 // Advanced AVX-512 operations
                 void butterfly_8(__m512d& a, __m512d& b, __m512d w);
                 __m512d twiddle_mul_8(__m512d data, __m512d twiddle);
             }
             
             // ARM NEON optimizations
             namespace neon {
                 // 64-bit integer operations (4 at once)
                 void add_4_int64(int64_t* a, int64_t* b, int64_t* result);
                 void mul_4_int64(int64_t* a, int64_t* b, int64_t* result);
                 
                 // Floating point operations
                 void add_4_double(double* a, double* b, double* result);
                 void mul_4_double(double* a, double* b, double* result);
             }
             
             // Performance monitoring
             namespace perf {
                 // Measure SIMD operation performance
                 double benchmark_simd_operation(const std::function<void()>& op, int iterations = 1000);
                 
                 // Compare SIMD vs scalar performance
                 void compare_simd_scalar(const std::string& operation_name);
             }
        
    } // namespace simd
    
} // namespace piracer 