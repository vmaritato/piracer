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
        }
        
    } // namespace simd
    
} // namespace piracer 