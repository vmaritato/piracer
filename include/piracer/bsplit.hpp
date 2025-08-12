#pragma once
#include <gmpxx.h>
#include "piracer/progress.hpp"

namespace piracer {
    // Minimal tuple used by binary-splitting.
    struct BSplitTriplet {
        mpz_class P;  // product of numerator polynomials
        mpz_class Q;  // product of denominator polynomials
        mpz_class T;  // combined series contribution
    };

    // Binary-splitting specialized for the Chudnovsky series on [a, b).
    BSplitTriplet bsplit_chudnovsky(long a, long b);

    // Same as above but reports progress at each leaf (terms).
    BSplitTriplet bsplit_chudnovsky(long a, long b, Progress* prog);
} // namespace piracer
