// src/alg/pi/chudnovsky.cpp
#include "piracer/chudnovsky.hpp"
#include "piracer/bsplit.hpp"
#include "piracer/format.hpp"

#include <cmath>
#include <gmpxx.h>
#include <mpfr.h>
#include <string>

namespace piracer {
    static inline double digits_per_term() { return 14.181647462725477; }

    static std::string compute_pi_impl(std::size_t digits, Progress* prog) {
        const long prec_bits =
            static_cast<long>(digits * 3.3219280948873626 + 64);
        const long n =
            static_cast<long>(std::ceil(digits / digits_per_term())) + 1;

        if (prog) { prog->done = 0; prog->total = static_cast<std::size_t>(n); }

        // Binary-splitting over [0, n) with optional progress
        BSplitTriplet S = prog
            ? bsplit_chudnovsky(0, n, prog)
            : bsplit_chudnovsky(0, n);

        // MPFR context
        mpfr_t pi, sqrt10005, tmp, qf, tf;
        mpfr_init2(pi, prec_bits);
        mpfr_init2(sqrt10005, prec_bits);
        mpfr_init2(tmp, prec_bits);
        mpfr_init2(qf, prec_bits);
        mpfr_init2(tf, prec_bits);

        mpfr_set_ui(sqrt10005, 10005u, MPFR_RNDN);
        mpfr_sqrt(sqrt10005, sqrt10005, MPFR_RNDN);

        mpfr_set_z(qf, S.Q.get_mpz_t(), MPFR_RNDN);

        mpz_class T_abs = S.T; if (T_abs < 0) T_abs = -T_abs;
        mpfr_set_z(tf, T_abs.get_mpz_t(), MPFR_RNDN);

        mpfr_mul_ui(tmp, sqrt10005, 426880u, MPFR_RNDN);
        mpfr_mul(tmp, tmp, qf, MPFR_RNDN);
        mpfr_div(pi, tmp, tf, MPFR_RNDN);

        std::string out = mpfr_to_fixed_decimal(pi, digits);
        mpfr_clears(pi, sqrt10005, tmp, qf, tf, (mpfr_ptr)0);
        return out;
    }

    std::string compute_pi(std::size_t digits) {
        return compute_pi_impl(digits, nullptr);
    }

    std::string compute_pi_with_progress(std::size_t digits, Progress* prog) {
        return compute_pi_impl(digits, prog);
    }
} // namespace piracer
