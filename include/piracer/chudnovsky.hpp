#pragma once
#include <cstddef>
#include <string>
#include "piracer/progress.hpp"

namespace piracer {
    // Compute π to `digits` decimals. No progress reporting.
    std::string compute_pi(std::size_t digits);

    // Same as above but reports progress via `prog` (ticks per term).
    std::string compute_pi_with_progress(std::size_t digits, Progress* prog);
} // namespace piracer
