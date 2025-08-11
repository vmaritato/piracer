#include <algorithm>   // std::min
#include <chrono>      // timings
#include <cmath>       // std::ceil, std::llround
#include <cstring>     // std::strlen
#include <fstream>
#include <gmpxx.h>
#include <iostream>
#include <mpfr.h>
#include <stdexcept>
#include <string>

// ============================================================================
// PiRacer — Baseline single-file (GMP/MPFR) for Chudnovsky + Binary-Splitting
// NOTE: This is a bootstrap file. In later sprints we will split into modules.
// ============================================================================

namespace piracer {

// ---- Chudnovsky constants ---------------------------------------------------
// Series term: (A + B*k)
// C is only needed through (C^3)/24 in the binary-splitting formulation.
static const mpz_class A = 13591409;
static const mpz_class B = 545140134;
// 640320^3 / 24 — keep as mpz_class to avoid overload ambiguity with GMP C++.
static const mpz_class C3_OVER_24 = mpz_class(640320) * 640320 * 640320 / 24;

// ---- Tuple for binary-splitting partials -----------------------------------
struct Triplet {
    mpz_class P;  // product of numerator polynomials
    mpz_class Q;  // product of denominator polynomials
    mpz_class T;  // combined series contribution
};

// ---- Binary-splitting for Chudnovsky on [a, b) ------------------------------
// Returns (P, Q, T) such that the partial sum over k in [a, b) can be combined
// with other intervals via:
//   P = P1 * P2
//   Q = Q1 * Q2
//   T = T1 * Q2 + P1 * T2
// NOTE: Use `long` here to avoid ambiguity of `long long` → mpz_class on macOS.
Triplet bsplit(long a, long b) {
    mpz_class az;                     // exact integer version of `a`
    mpz_set_si(az.get_mpz_t(), a);    // avoid ambiguous ctor from long long

    if (b - a == 1) {
        // Base case for a single term
        if (a == 0) {
            Triplet r;
            r.P = 1;
            r.Q = 1;
            r.T = A;
            return r;
        }
        // P(a) = (6a-5)(2a-1)(6a-1)
        mpz_class P = (6 * az - 5) * (2 * az - 1) * (6 * az - 1);
        // Q(a) = a^3 * (C^3 / 24)
        mpz_class Q = az * az * az;
        Q *= C3_OVER_24;
        // T(a) = P(a) * (A + B*a) with alternating sign (-1)^a
        mpz_class T = P * (A + B * az);
        if (a & 1) T = -T;
        return Triplet{P, Q, T};
    }

    // Recursive split
    long m = (a + b) / 2;
    Triplet left  = bsplit(a, m);
    Triplet right = bsplit(m, b);

    Triplet res;
    res.P = left.P * right.P;
    res.Q = left.Q * right.Q;
    res.T = left.T * right.Q + left.P * right.T;
    return res;
}

// ---- Digits per term (empirical constant for Chudnovsky) --------------------
inline double digits_per_term() { return 14.181647462725477; }

// ---- Convert mpfr_t to fixed "X.Y..." with exactly `digits` decimals --------
// MPFR returns a mantissa+exponent pair in base 10. We stitch it into a
// human-friendly fixed-point decimal string.
std::string mpfr_to_fixed_decimal(const mpfr_t v, size_t digits) {
    mp_exp_t expo;
    // Ask for a couple extra digits to avoid rounding issues at the tail.
    char* buf = mpfr_get_str(nullptr, &expo, 10, digits + 2, v, MPFR_RNDN);
    if (!buf) throw std::runtime_error("mpfr_get_str failed");

    std::string out;
    const bool neg = (buf[0] == '-');
    char* mant = buf + (neg ? 1 : 0);

    if (neg) out.push_back('-');

    if (expo <= 0) {
        // 0.[zeros]mant
        out += "0.";
        out.append(static_cast<size_t>(-expo), '0');
        out += mant;
    } else {
        size_t len = std::strlen(mant);
        if ((mp_exp_t)len <= expo) {
            // mant[0:len] + zeros + "."
            out.append(mant, mant + len);
            out.append(static_cast<size_t>(expo - len), '0');
            out += ".";
        } else {
            // mant[0:expo] + "." + mant[expo:expo+digits]
            out.append(mant, mant + expo);
            out += ".";
            out.append(mant + expo,
                       mant + std::min(len, (size_t)(expo + (mp_exp_t)digits)));
        }
    }

    // Enforce exactly `digits` decimals
    auto pos = out.find('.');
    if (pos == std::string::npos) {
        out += ".";
        pos = out.size() - 1;
    }
    size_t have = out.size() - pos - 1;
    if (have < digits) out.append(digits - have, '0');
    if (have > digits) out.erase(pos + 1 + digits);

    mpfr_free_str(buf);
    return out;
}

// ---- Compute pi to `digits` decimals via Chudnovsky + binary-splitting ------
// pi = (426880 * sqrt(10005) * Q) / |T|
std::string compute_pi(size_t digits) {
    // Precision in bits ~= digits * log2(10) + safety margin
    const long prec_bits =
        static_cast<long>(digits * 3.3219280948873626 + 64);

    // Number of terms required
    long n = static_cast<long>(std::ceil(digits / digits_per_term())) + 1;

    Triplet R = bsplit(0, n);

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
    mpfr_set_z(qf, R.Q.get_mpz_t(), MPFR_RNDN);

    // tf = |T| (positive denominator)
    mpz_class T_abs = R.T;
    if (T_abs < 0) T_abs = -T_abs;
    mpfr_set_z(tf, T_abs.get_mpz_t(), MPFR_RNDN);

    // tmp = 426880 * sqrt(10005)
    mpfr_mul_ui(tmp, sqrt10005, 426880u, MPFR_RNDN);

    // tmp = qf * tmp
    mpfr_mul(tmp, tmp, qf, MPFR_RNDN);

    // pi = tmp / tf
    mpfr_div(pi, tmp, tf, MPFR_RNDN);

    std::string s = mpfr_to_fixed_decimal(pi, digits);

    mpfr_clears(pi, sqrt10005, tmp, qf, tf, (mpfr_ptr)0);
    return s;
}

} // namespace piracer

// ============================================================================
// CLI helpers — neat text design, strict errors, and clear examples
// ============================================================================

static constexpr const char* kVersion = "0.1.0";

static std::string basename_of(const char* argv0) {
    std::string s = argv0 ? argv0 : "piracer";
    auto pos = s.find_last_of("/\\");
    return (pos == std::string::npos) ? s : s.substr(pos + 1);
}

// Parse integers like "1000000" and scientific-like strings "1e6".
static size_t parse_digits(const std::string& s) {
    try {
        if (s.find_first_of("eE") != std::string::npos) {
            long double v = std::stold(s);
            if (!(v > 0.0L)) throw std::invalid_argument("digits must be > 0");
            long double r = std::llround(v);
            if (r < 1.0L) r = 1.0L;
            return static_cast<size_t>(r);
        }
        return static_cast<size_t>(std::stoull(s));
    } catch (...) {
        throw std::invalid_argument("invalid value for --digits: '" + s + "'");
    }
}

static void print_banner() {
    std::cerr
        << "PiRacer — Fast, reproducible π (baseline)\n"
        << "Chudnovsky + Binary-Splitting (GMP/MPFR)\n";
}

static void print_version() {
    std::cerr << "PiRacer " << kVersion << " (baseline GMP/MPFR)\n";
}

static void print_help(const char* argv0) {
    const std::string me = basename_of(argv0);
    print_banner();
    std::cerr
        << "\nUSAGE\n"
        << "  " << me << " --digits N [--out FILE] [--quiet]\n"
        << "  " << me << " -n N        [-o FILE] [-q]\n"
        << "\nOPTIONS\n"
        << "  -n, --digits N    Number of decimal digits to compute (required).\n"
        << "                    Accepts forms like 1000000 or 1e6.\n"
        << "  -o, --out FILE    Write to FILE instead of stdout (optional).\n"
        << "  -q, --quiet       Suppress non-result logs (stderr) (optional).\n"
        << "  -V, --version     Show version and exit.\n"
        << "  -h, --help        Show this help and exit.\n"
        << "\nEXAMPLES\n"
        << "  " << me << " --digits 100000 > pi.txt\n"
        << "  " << me << " -n 1e6 -o pi_1M.txt\n"
        << "\nNOTES\n"
        << "  • Logs and timings are printed to stderr; π digits go to stdout or FILE.\n"
        << "  • This is the baseline implementation. Future sprints add NTT, threads, etc.\n";
}

// ============================================================================
// main
// ============================================================================
int main(int argc, char** argv) {
    try {
        size_t digits = 0;
        std::string out;
        bool quiet = false;

        // Simple manual parsing (robust enough for baseline)
        for (int i = 1; i < argc; ++i) {
            std::string a = argv[i];
            if ((a == "--digits" || a == "-n") && i + 1 < argc) {
                digits = parse_digits(argv[++i]);
            } else if ((a == "--out" || a == "-o") && i + 1 < argc) {
                out = argv[++i];
            } else if (a == "--quiet" || a == "-q") {
                quiet = true;
            } else if (a == "--version" || a == "-V") {
                print_version();
                return 0;
            } else if (a == "--help" || a == "-h" || a == "help") {
                print_help(argv[0]);
                return 0;
            } else {
                std::cerr << "Unknown option: " << a << "\n";
                std::cerr << "Try '--help' for usage.\n";
                return 1;
            }
        }

        if (digits == 0) {
            std::cerr << "Missing required option: --digits N\n";
            std::cerr << "Try '--help' for usage.\n";
            return 1;
        }

        using clock = std::chrono::high_resolution_clock;
        auto t0 = clock::now();

        if (!quiet) {
            print_banner();
            std::cerr << "Request: " << digits << " decimal digits\n";
        }

        std::string pi = piracer::compute_pi(digits);

        // Output π either to stdout or file, keep logs on stderr
        if (out.empty()) {
            std::cout << pi << std::endl;
        } else {
            std::ofstream f(out);
            if (!f) throw std::runtime_error("cannot open output file: " + out);
            f << pi << "\n";
            f.close();
        }

        auto t1 = clock::now();
        std::chrono::duration<double> dt = t1 - t0;

        if (!quiet) {
            if (!out.empty())
                std::cerr << "Wrote " << digits << " digits to '" << out << "'\n";
            std::cerr << "Elapsed: " << dt.count() << " s\n";
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        std::cerr << "Tip: run with '--help' for usage.\n";
        return 2;
    }
}
