#pragma once
#include <cstddef>
#include <string>

namespace piracer {
    // Compute π with Chudnovsky + binary-splitting up to `digits` decimal digits.
    std::string compute_pi(std::size_t digits);
} // namespace piracer
