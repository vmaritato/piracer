#include "piracer/selftest.hpp"
#include "piracer/format.hpp"
#include "piracer/chudnovsky.hpp"

#include <mpfr.h>
#include <string>

namespace piracer {
    bool self_test(std::size_t digits, std::string* message) {
        // Enough precision for `digits` decimals + some guard bits
        const long prec_bits = static_cast<long>(digits * 3.3219280948873626 + 64);

        // Reference π from MPFR (independent from our series)
        mpfr_t ref;
        mpfr_init2(ref, prec_bits);
        mpfr_const_pi(ref, MPFR_RNDN);
        std::string expected = mpfr_to_fixed_decimal(ref, digits);
        mpfr_clear(ref);

        // Our implementation
        std::string got = compute_pi(digits);

        bool ok = (got == expected);
        if (message) {
            if (ok) {
                *message = "OK - outputs match exactly";
            } else {
                // Find first mismatch index (small but informative)
                std::size_t i = 0, n = std::min(got.size(), expected.size());
                while (i < n && got[i] == expected[i]) ++i;
                *message = "Mismatch at char index " + std::to_string(i);
            }
        }
        return ok;
    }
} // namespace piracer