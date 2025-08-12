#include "piracer/bsplit.hpp"

namespace piracer {
    namespace {
    // Chudnovsky constants used by the bsplit leaf.
    // Kept in an anonymous namespace (internal linkage).
    const mpz_class A = 13591409;
    const mpz_class B = 545140134;
    // 640320^3 / 24 — stay as mpz_class to avoid ctor/operator ambiguity.
    const mpz_class C3_OVER_24 = mpz_class(640320) * 640320 * 640320 / 24;
    } // namespace

    // Binary-splitting for Chudnovsky on [a, b).
    BSplitTriplet bsplit_chudnovsky(long a, long b) {
        mpz_class az;                     // exact integer version of `a`
        mpz_set_si(az.get_mpz_t(), a);    // avoids ambiguous long long constructor

        if (b - a == 1) {
            if (a == 0) {
                return BSplitTriplet{ mpz_class{1}, mpz_class{1}, A };
            }
            // P(a) = (6a-5)(2a-1)(6a-1)
            mpz_class P = (6 * az - 5) * (2 * az - 1) * (6 * az - 1);
            // Q(a) = a^3 * (C^3 / 24)
            mpz_class Q = az * az * az;
            Q *= C3_OVER_24;
            // T(a) = P(a) * (A + B*a) with alternating sign (-1)^a
            mpz_class T = P * (A + B * az);
            if (a & 1) T = -T;
            return BSplitTriplet{P, Q, T};
        }

        long m = (a + b) / 2;
        BSplitTriplet L = bsplit_chudnovsky(a, m);
        BSplitTriplet R = bsplit_chudnovsky(m, b);

        BSplitTriplet res;
        res.P = L.P * R.P;
        res.Q = L.Q * R.Q;
        res.T = L.T * R.Q + L.P * R.T;
        return res;
    }
} // namespace piracer
