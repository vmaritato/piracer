#pragma once
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <string>

namespace piracer {
    // Parse integers like "1000000" and scientific-like strings "1e6" into size_t.
    // Guarantees digits >= 1.
    inline std::size_t parse_digits(const std::string& s) {
        if (s.find_first_of("eE") != std::string::npos) {
            long double v = std::stold(s);
            if (!(v > 0.0L)) throw std::invalid_argument("digits must be > 0");
            long double r = std::llround(v);
            if (r < 1.0L) r = 1.0L;
            return static_cast<std::size_t>(r);
        }
        return static_cast<std::size_t>(std::stoull(s));
    }
} // namespace piracer
