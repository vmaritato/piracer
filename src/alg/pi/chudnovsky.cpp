#include "piracer/chudnovsky.hpp"
#include "piracer/format.hpp"

#include <algorithm>
#include <cmath>
#include <gmpxx.h>
#include <mpfr.h>
#include <stdexcept>
#include <string>

namespace piracer {
    // --- Chudnovsky constants ----------------------------------------------------
    static const mpz_class A = 13591409;
    static const mpz_class B = 545140134;
    // 640320^3 / 24 — keep as mpz_class to avoid overload ambiguity.
    static const mpz_class C3_OVER_24 = mpz_class(640320) * 640320 * 640320 / 24;

    // Digits per term (empirical constant for Chudnovsky)
    static inline double digits_per_term() { return 14.181647462725477; }

    // Internal tuple used by binary-splitting.
    struct Triplet {
        mpz_class P;  // product of numerator polynomials
        mpz_class Q;  // product of denominator polynomials
        mpz_class T;  // combined series contribution
    };

    // Binary-splitting on interval [a, b). We use `long` to avoid ctor ambiguity on macOS.
    static Triplet bsplit(long a, long b) {
        mpz_class az;                       // exact integer version of `a`
        mpz_set_si(az.get_mpz_t(), a);      // avoids ambiguous long long constructor

        if (b - a == 1) {
            if (a == 0) {
                Triplet r; r.P = 1; r.Q = 1; r.T = A; return r;
            }
            // P(a) = (6a-5)(2a-1)(6a-1)
            mpz_class P = (6 * az - 5) * (2 * az - 1) * (6 * az - 1);
            // Q(a) = a^3 * (C^3/24)
            mpz_class Q = az * az * az;
            Q *= C3_OVER_24;
            // T(a) = P(a) * (A + B*a) with alternating sign (-1)^a
            mpz_class T = P * (A + B * az);
            if (a & 1) T = -T;
            return Triplet{P, Q, T};
        }

        long m = (a + b) / 2;
        Triplet L = bsplit(a, m);
        Triplet R = bsplit(m, b);

        Triplet res;
        res.P = L.P * R.P;
        res.Q = L.Q * R.Q;
        res.T = L.T * R.Q + L.P * R.T;
        return res;
    }

    std::string compute_pi(std::size_t digits) {
        // Precision in bits ~= digits * log2(10) + safety margin
        const long prec_bits = static_cast<long>(digits * 3.3219280948873626 + 64);

        // Number of terms required
        const long n = static_cast<long>(std::ceil(digits / digits_per_term())) + 1;

        Triplet S = bsplit(0, n);

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

        // qf = Q
        mpfr_set_z(qf, S.Q.get_mpz_t(), MPFR_RNDN);

        // tf = |T| (positive denominator)
        mpz_class T_abs = S.T; if (T_abs < 0) T_abs = -T_abs;
        mpfr_set_z(tf, T_abs.get_mpz_t(), MPFR_RNDN);

        // tmp = 426880 * sqrt(10005)
        mpfr_mul_ui(tmp, sqrt10005, 426880u, MPFR_RNDN);

        // tmp = qf * tmp
        mpfr_mul(tmp, tmp, qf, MPFR_RNDN);

        // pi = tmp / tf
        mpfr_div(pi, tmp, tf, MPFR_RNDN);

        std::string out = mpfr_to_fixed_decimal(pi, digits);

        mpfr_clears(pi, sqrt10005, tmp, qf, tf, (mpfr_ptr)0);
        return out;
    }
} // namespace piracer
