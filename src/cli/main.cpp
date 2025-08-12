#include "piracer/chudnovsky.hpp"
#include "piracer/cli_utils.hpp"
#include "piracer/version.hpp"
#include "piracer/selftest.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

// ============================================================================
// PiRacer — Thin CLI (baseline)
// Handles parsing, help/version, self-test shortcut, timings, and I/O plumbing.
// ============================================================================

namespace {
std::string basename_of(const char* argv0) {
    std::string s = argv0 ? argv0 : "piracer";
    auto pos = s.find_last_of("/\\");
    return (pos == std::string::npos) ? s : s.substr(pos + 1);
}

void print_banner() {
    std::cerr << "PiRacer — Fast, reproducible π (baseline)\n"
              << "Chudnovsky + Binary-Splitting (GMP/MPFR)\n";
}

void print_version() {
    std::cerr << "PiRacer " << piracer::version << " (baseline GMP/MPFR)\n";
}

void print_help(const char* argv0) {
    const std::string me = basename_of(argv0);
    print_banner();
    std::cerr
        << "\nUSAGE\n"
        << "  " << me << " --digits N [--out FILE] [--quiet]\n"
        << "  " << me << " -n N        [-o FILE] [-q]\n"
        << "  " << me << " --self-test [--digits N]\n"
        << "  " << me << " -T          [-n N]\n"
        << "\nOPTIONS\n"
        << "  -n, --digits N    Number of decimal digits to compute.\n"
        << "                    Accepts forms like 1000000 or 1e6.\n"
        << "  -o, --out FILE    Write to FILE instead of stdout.\n"
        << "  -q, --quiet       Suppress non-result logs (stderr).\n"
        << "  -T, --self-test   Run a correctness self-test (defaults to 1000 digits;\n"
        << "                    respects --digits if provided) and exit.\n"
        << "  -V, --version     Show version and exit.\n"
        << "  -h, --help        Show this help and exit.\n"
        << "\nEXAMPLES\n"
        << "  " << me << " --digits 100000 > pi.txt\n"
        << "  " << me << " -n 1e6 -o pi_1M.txt\n"
        << "  " << me << " --self-test          # defaults to 1000 digits\n"
        << "  " << me << " --self-test -n 2500  # test at 2500 digits\n";
}
} // namespace

int main(int argc, char** argv) {
    try {
        std::size_t digits = 0;
        std::string out;
        bool quiet = false;
        bool do_selftest = false;

        // Simple manual parsing (robust enough for baseline)
        for (int i = 1; i < argc; ++i) {
            std::string a = argv[i];
            if ((a == "--digits" || a == "-n") && i + 1 < argc) {
                digits = piracer::parse_digits(argv[++i]);
            } else if ((a == "--out" || a == "-o") && i + 1 < argc) {
                out = argv[++i];
            } else if (a == "--quiet" || a == "-q") {
                quiet = true;
            } else if (a == "--self-test" || a == "-T") {
                do_selftest = true;
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

        // Handle self-test BEFORE enforcing --digits.
        if (do_selftest) {
            const std::size_t k = digits ? digits : 1000;
            if (!quiet) {
                print_banner();
                std::cerr << "Running self-test at " << k << " digits...\n";
            }
            std::string verdict;
            bool ok = piracer::self_test(k, &verdict);
            std::cerr << "Self-test: " << (ok ? "OK ✅" : "FAIL ❌")
                      << (verdict.empty() ? "" : " — " + verdict) << "\n";
            return ok ? 0 : 3;
        }

        // Regular compute mode requires --digits.
        if (digits == 0) {
            std::cerr << "Missing required option: --digits N\n";
            std::cerr << "Tip: you can also run '--self-test' (defaults to 1000 digits).\n";
            return 1;
        }

        using clock = std::chrono::high_resolution_clock;
        auto t0 = clock::now();

        if (!quiet) {
            print_banner();
            std::cerr << "Request: " << digits << " decimal digits\n";
        }

        std::string pi = piracer::compute_pi(digits);

        // Output π either to stdout or file, keep logs on stderr.
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
        std::cerr << "Error: " << e.what() << "\n"
                  << "Tip: run with '--help' for usage.\n";
        return 2;
    }
}
