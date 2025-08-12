#include "piracer/format.hpp"
#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace piracer {
    std::string mpfr_to_fixed_decimal(const mpfr_t v, std::size_t digits) {
        mp_exp_t expo;
        // Request a couple guard digits; MPFR will round correctly at given precision.
        char* buf = mpfr_get_str(nullptr, &expo, 10, digits + 2, v, MPFR_RNDN);
        if (!buf) throw std::runtime_error("mpfr_get_str failed");

        std::string out;
        const bool neg = (buf[0] == '-');
        char* mant = buf + (neg ? 1 : 0);

        if (neg) out.push_back('-');

        if (expo <= 0) {
            out += "0.";
            out.append(static_cast<std::size_t>(-expo), '0');
            out += mant;
        } else {
            std::size_t len = std::strlen(mant);
            if ((mp_exp_t)len <= expo) {
                out.append(mant, mant + len);
                out.append(static_cast<std::size_t>(expo - len), '0');
                out += ".";
            } else {
                out.append(mant, mant + expo);
                out += ".";
                out.append(mant + expo, mant + std::min(len, (std::size_t)(expo + (mp_exp_t)digits)));
            }
        }

        // Enforce exactly `digits` decimals
        auto pos = out.find('.');
        if (pos == std::string::npos) {
            out += ".";
            pos = out.size() - 1;
        }
        std::size_t have = out.size() - pos - 1;
        if (have < digits) out.append(digits - have, '0');
        if (have > digits) out.erase(pos + 1 + digits);

        mpfr_free_str(buf);
        return out;
    }
} // namespace piracer
