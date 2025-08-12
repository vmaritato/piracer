#pragma once
#include <gmpxx.h>

namespace piracer {
    // Minimal tuple used by binary-splitting.
    struct BSplitTriplet {
        mpz_class P;  // product of numerator polynomials
        mpz_class Q;  // product of denominator polynomials
        mpz_class T;  // combined series contribution
    };

    // Binary-splitting specialized for the Chudnovsky series on [a, b).
    // This isolates the math rules so the caller (compute_pi) stays simple.
    BSplitTriplet bsplit_chudnovsky(long a, long b);
} // namespace piracer
