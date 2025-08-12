#include "piracer/bsplit.hpp"

namespace piracer {
    namespace {
        const mpz_class A = 13591409;
        const mpz_class B = 545140134;
        const mpz_class C3_OVER_24 = mpz_class(640320) * 640320 * 640320 / 24;

        // Leaf computation shared by both entry points
        inline BSplitTriplet leaf(long a) {
            if (a == 0) return BSplitTriplet{ mpz_class{1}, mpz_class{1}, A };

            mpz_class az; mpz_set_si(az.get_mpz_t(), a);
            mpz_class P = (6 * az - 5) * (2 * az - 1) * (6 * az - 1);
            mpz_class Q = az * az * az; Q *= C3_OVER_24;
            mpz_class T = P * (A + B * az);
            if (a & 1) T = -T;
            return BSplitTriplet{P, Q, T};
        }
    } // namespace

    // Non-reporting version
    static BSplitTriplet bsplit_impl(long a, long b) {
        if (b - a == 1) return leaf(a);
        long m = (a + b) / 2;
        BSplitTriplet L = bsplit_impl(a, m);
        BSplitTriplet R = bsplit_impl(m, b);
        return BSplitTriplet{
            L.P * R.P,
            L.Q * R.Q,
            L.T * R.Q + L.P * R.T
        };
    }

    // Reporting version (ticks at each leaf)
    static BSplitTriplet bsplit_impl(long a, long b, Progress* prog) {
        if (b - a == 1) {
            BSplitTriplet x = leaf(a);
            if (prog) {
                ++prog->done;
                if (prog->tick) prog->tick(prog->done, prog->total, prog->user);
            }
            return x;
        }
        long m = (a + b) / 2;
        BSplitTriplet L = bsplit_impl(a, m, prog);
        BSplitTriplet R = bsplit_impl(m, b, prog);
        return BSplitTriplet{
            L.P * R.P,
            L.Q * R.Q,
            L.T * R.Q + L.P * R.T
        };
    }

    BSplitTriplet bsplit_chudnovsky(long a, long b) {
        return bsplit_impl(a, b);
    }

    BSplitTriplet bsplit_chudnovsky(long a, long b, Progress* prog) {
        return bsplit_impl(a, b, prog);
    }
} // namespace piracer