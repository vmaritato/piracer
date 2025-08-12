#pragma once
#include <cstddef>

namespace piracer {
    // Lightweight progress reporting for long-running computations.
    // The core increments `done` and optionally calls `tick(done,total,user)`.
    // The caller (CLI) owns rendering/ETA via `user`.
    struct Progress {
        std::size_t total = 0;
        std::size_t done  = 0;
        void (*tick)(std::size_t done, std::size_t total, void* user) = nullptr;
        void* user = nullptr;
    };
} // namespace piracer
