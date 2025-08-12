#pragma once
#include <string>
#include <cmath>
#include <cstddef>

namespace piracer {
    // Parse integers like "1000000" and scientific-like strings "1e6".
    inline std::size_t parse_integer(const std::string& s) {
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