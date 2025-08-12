#pragma once
#include <gmpxx.h>
#include "piracer/progress.hpp"

namespace piracer {
    // Minimal tuple used by binary-splitting.
    struct BSplitTriplet {
        mpz_class P;  // product of numerator polynomials
        mpz_class Q;  // product of denominator polynomials
        mpz_class T;  // combined series contribution
    };

    // Binary-splitting specialized for the Chudnovsky series on [a, b).
    BSplitTriplet bsplit_chudnovsky(long a, long b);

    // Same as above but reports progress at each leaf (terms).
    BSplitTriplet bsplit_chudnovsky(long a, long b, Progress* prog);
    
    // Parallel binary-splitting with simple scheduler
    BSplitTriplet bsplit_chudnovsky_parallel(long a, long b, int num_threads, Progress* prog = nullptr);
    
    // Simple task scheduler for parallel bsplit
    struct ParallelScheduler {
        int num_threads;
        long chunk_size;
        
        ParallelScheduler(int threads, long chunk = 1000);
        
        // Get next chunk to process
        std::pair<long, long> get_next_chunk();
        
        // Check if there are more chunks
        bool has_more_chunks() const;
        
    private:
        long current_pos;
        long end_pos;
    };
} // namespace piracer
