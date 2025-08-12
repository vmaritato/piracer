#include "piracer/bsplit.hpp"
#include "piracer/thread_pool.hpp"

namespace piracer {
    namespace {
        const mpz_class A = 13591409;
        const mpz_class B = 545140134;
        const mpz_class C3_OVER_24 = mpz_class(640320) * 640320 * 640320 / 24;

        // Leaf computation shared by both entry points
        inline BSplitTriplet leaf(long a) {
            if (a == 0) return BSplitTriplet{ mpz_class{1}, mpz_class{1}, A };

            mpz_class az; mpz_set_si(az.get_mpz_t(), a);
            mpz_class P = (6 * az - 5) * (2 * az - 1) * (6 * az - 1);
            mpz_class Q = az * az * az; Q *= C3_OVER_24;
            mpz_class T = P * (A + B * az);
            if (a & 1) T = -T;
            return BSplitTriplet{P, Q, T};
        }
    } // namespace

    // Non-reporting version
    static BSplitTriplet bsplit_impl(long a, long b) {
        if (b - a == 1) return leaf(a);
        long m = (a + b) / 2;
        BSplitTriplet L = bsplit_impl(a, m);
        BSplitTriplet R = bsplit_impl(m, b);
        return BSplitTriplet{
            L.P * R.P,
            L.Q * R.Q,
            L.T * R.Q + L.P * R.T
        };
    }

    // Reporting version (ticks at each leaf)
    static BSplitTriplet bsplit_impl(long a, long b, Progress* prog) {
        if (b - a == 1) {
            BSplitTriplet x = leaf(a);
            if (prog) {
                ++prog->done;
                if (prog->tick) prog->tick(prog->done, prog->total, prog->user);
            }
            return x;
        }
        long m = (a + b) / 2;
        BSplitTriplet L = bsplit_impl(a, m, prog);
        BSplitTriplet R = bsplit_impl(m, b, prog);
        return BSplitTriplet{
            L.P * R.P,
            L.Q * R.Q,
            L.T * R.Q + L.P * R.T
        };
    }

    BSplitTriplet bsplit_chudnovsky(long a, long b) {
        return bsplit_impl(a, b);
    }

    BSplitTriplet bsplit_chudnovsky(long a, long b, Progress* prog) {
        return bsplit_impl(a, b, prog);
    }
    
    // Parallel scheduler implementation
    ParallelScheduler::ParallelScheduler(int threads, long chunk)
        : num_threads(threads), chunk_size(chunk), current_pos(0), end_pos(0) {
        if (threads > 1) {
            thread_pool = std::make_unique<ThreadPool>(threads);
        }
    }
    
    ParallelScheduler::~ParallelScheduler() = default;
    
    std::pair<long, long> ParallelScheduler::get_next_chunk() {
        if (current_pos >= end_pos) {
            return {-1, -1};  // No more chunks
        }
        
        long start = current_pos;
        long end = std::min(current_pos + chunk_size, end_pos);
        current_pos = end;
        
        return {start, end};
    }
    
    bool ParallelScheduler::has_more_chunks() const {
        return current_pos < end_pos;
    }
    
    ThreadPool* ParallelScheduler::get_thread_pool() const {
        return thread_pool.get();
    }
    
    // Parallel binary-splitting implementation
    BSplitTriplet bsplit_chudnovsky_parallel(long a, long b, int num_threads, Progress* prog) {
        // For now, this is a stub that falls back to sequential
        // TODO: Implement actual parallel processing with thread pool
        
        if (num_threads <= 1) {
            return bsplit_chudnovsky(a, b, prog);
        }
        
        // Simple chunk-based approach (not yet parallel)
        const long chunk_size = std::max(1L, (b - a) / num_threads);
        ParallelScheduler scheduler(num_threads, chunk_size);
        
        // For now, just process sequentially but with chunking
        BSplitTriplet result{mpz_class{1}, mpz_class{1}, mpz_class{0}};
        
        for (long chunk_start = a; chunk_start < b; chunk_start += chunk_size) {
            long chunk_end = std::min(chunk_start + chunk_size, b);
            BSplitTriplet chunk = bsplit_chudnovsky(chunk_start, chunk_end, prog);
            
            // Combine results
            result.P *= chunk.P;
            result.Q *= chunk.Q;
            result.T = result.T * chunk.Q + result.P * chunk.T;
        }
        
        return result;
    }
} // namespace piracer