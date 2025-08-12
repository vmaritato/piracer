// src/alg/pi/chudnovsky.cpp
//
// PiRacer — Chudnovsky π computation (baseline)
// This module exposes `compute_pi(digits)` which evaluates π to a given
// number of decimal digits using the Chudnovsky series combined with a
// binary-splitting engine (implemented in bsplit.cpp).
//
// Responsibilities in this file:
//   • choose the required precision/term count
//   • call the binary-splitting routine specialized for Chudnovsky
//   • assemble the final value of π with MPFR
//   • return a fixed-point decimal string with exactly `digits` decimals
//
// Note: Heavy integer work (P/Q/T products) is performed by GMP (via mpz_class).
//       Final high-precision floating arithmetic (sqrt/div) is performed by MPFR.

#include "piracer/chudnovsky.hpp"
#include "piracer/bsplit.hpp"
#include "piracer/format.hpp"

#include <cmath>      // std::ceil
#include <gmpxx.h>    // mpz_class
#include <mpfr.h>     // MPFR API
#include <string>

namespace piracer {
    // ----------------------------------------------------------------------------
    // Empirical constant: decimal digits gained per Chudnovsky term
    // Reference value ~14.1816474627...
    // ----------------------------------------------------------------------------
    static inline double digits_per_term() { return 14.181647462725477; }

    // ----------------------------------------------------------------------------
    // compute_pi
    //   Compute π with Chudnovsky + binary-splitting up to `digits` decimal digits.
    //   Returns a fixed-point string: "3.<digits digits>".
    // ----------------------------------------------------------------------------
    std::string compute_pi(std::size_t digits) {
        // MPFR precision (in bits): digits * log2(10) + safety margin.
        const long prec_bits =
            static_cast<long>(digits * 3.3219280948873626 + 64);

        // Number of Chudnovsky terms required to reach `digits` decimals.
        const long n =
            static_cast<long>(std::ceil(digits / digits_per_term())) + 1;

        // Binary-splitting over [0, n): compute (P, Q, T) triplet.
        // For Chudnovsky, the final formula is:
        //   π = (426880 * sqrt(10005) * Q) / |T|
        const BSplitTriplet S = bsplit_chudnovsky(0, n);

        // MPFR context
        mpfr_t pi, sqrt10005, tmp, qf, tf;
        mpfr_init2(pi, prec_bits);
        mpfr_init2(sqrt10005, prec_bits);
        mpfr_init2(tmp, prec_bits);
        mpfr_init2(qf, prec_bits);
        mpfr_init2(tf, prec_bits);

        // sqrt(10005)
        mpfr_set_ui(sqrt10005, 10005u, MPFR_RNDN);
        mpfr_sqrt(sqrt10005, sqrt10005, MPFR_RNDN);

        // qf = Q (as MPFR)
        mpfr_set_z(qf, S.Q.get_mpz_t(), MPFR_RNDN);

        // tf = |T| (positive denominator as MPFR)
        mpz_class T_abs = S.T;
        if (T_abs < 0) T_abs = -T_abs;
        mpfr_set_z(tf, T_abs.get_mpz_t(), MPFR_RNDN);

        // tmp = 426880 * sqrt(10005)
        mpfr_mul_ui(tmp, sqrt10005, 426880u, MPFR_RNDN);

        // tmp = qf * tmp
        mpfr_mul(tmp, tmp, qf, MPFR_RNDN);

        // pi = tmp / tf
        mpfr_div(pi, tmp, tf, MPFR_RNDN);

        // Convert to fixed decimal string "X.Y..." with exactly `digits` decimals.
        std::string out = mpfr_to_fixed_decimal(pi, digits);

        // Cleanup
        mpfr_clears(pi, sqrt10005, tmp, qf, tf, (mpfr_ptr)0);
        return out;
    }
} // namespace piracer
