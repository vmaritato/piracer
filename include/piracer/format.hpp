#pragma once
#include <cstddef>
#include <string>
#include <mpfr.h>

namespace piracer {
    // Convert an MPFR value to a fixed-point decimal string "X.Y..." with exactly `digits` decimals.
    std::string mpfr_to_fixed_decimal(const mpfr_t v, std::size_t digits);
} // namespace piracer