#include "piracer/chudnovsky.hpp"
#include "piracer/cli_utils.hpp"
#include "piracer/version.hpp"
#include "piracer/selftest.hpp"
#include "piracer/progress.hpp"

#include <iomanip> // setw, setprecision
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
        << "  " << me << " --digits N [--out FILE] [--base {dec,hex}] [--threads N] [--quiet]\n"
        << "  " << me << " -n N        [-o FILE] [-b {dec,hex}] [-t N] [-q]\n"
        << "  " << me << " --self-test [--digits N]\n"
        << "  " << me << " -T          [-n N]\n"
        << "\nOPTIONS\n"
        << "  -n, --digits N    Number of decimal digits to compute.\n"
        << "                    Accepts forms like 1000000 or 1e6.\n"
        << "  -o, --out FILE    Write to FILE instead of stdout.\n"
        << "  -b, --base BASE   Output base: dec (decimal) or hex (hexadecimal).\n"
        << "                    Default: dec\n"
        << "  -t, --threads N   Number of threads (no-op for now, future NTT support).\n"
        << "                    Default: 1\n"
        << "  -q, --quiet       Suppress non-result logs (stderr).\n"
        << "  -p, --progress    Show a live progress bar with ETA during computation.\n"
        << "  -T, --self-test   Run a correctness self-test (defaults to 1000 digits;\n"
        << "                    respects --digits if provided) and exit.\n"
        << "  -V, --version     Show version and exit.\n"
        << "  -h, --help        Show this help and exit.\n"
        << "\nEXAMPLES\n"
        << "  " << me << " --digits 100000 > pi.txt\n"
        << "  " << me << " -n 1e6 -o pi_1M.txt\n"
        << "  " << me << " --base hex -n 1000    # output in hexadecimal\n"
        << "  " << me << " --self-test          # defaults to 1000 digits\n"
        << "  " << me << " --self-test -n 2500  # test at 2500 digits\n";
}
} // namespace

int main(int argc, char** argv) {
    try {
        std::size_t digits = 0;
        std::string out;
        int base = 10;  // default to decimal
        int threads = 1;  // default to single thread
        bool quiet = false;
        bool do_selftest = false;
        bool show_progress = false;
        
        // Simple manual parsing (robust enough for baseline)
        for (int i = 1; i < argc; ++i) {
            std::string a = argv[i];
            if ((a == "--digits" || a == "-n") && i + 1 < argc) {
                digits = piracer::parse_digits(argv[++i]);
            } else if ((a == "--out" || a == "-o") && i + 1 < argc) {
                out = argv[++i];
            } else if ((a == "--base" || a == "-b") && i + 1 < argc) {
                std::string b = argv[++i];
                if (b == "dec" || b == "decimal") {
                    base = 10;
                } else if (b == "hex" || b == "hexadecimal") {
                    base = 16;
                } else {
                    std::cerr << "Invalid base: " << b << " (use 'dec' or 'hex')\n";
                    return 1;
                }
            } else if ((a == "--threads" || a == "-t") && i + 1 < argc) {
                threads = std::stoi(argv[++i]);
                if (threads < 1) {
                    std::cerr << "Invalid thread count: " << threads << " (must be >= 1)\n";
                    return 1;
                }
            } else if (a == "--quiet" || a == "-q") {
                quiet = true;
            } else if (a == "--self-test" || a == "-T") {
                do_selftest = true;
            } else if (a == "--progress" || a == "-p") {
                show_progress = true;
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
            std::cerr << "Request: " << digits << " " << (base == 16 ? "hexadecimal" : "decimal") << " digits\n";
            if (threads > 1) {
                std::cerr << "Threads: " << threads << " (no-op for now, future NTT support)\n";
            }
        }

        // Optional progress bar: tick per series term
        std::string pi;
        if (show_progress && !quiet) {
            struct Bar {
                std::chrono::steady_clock::time_point start, last;
                int width = 40;
            } bar { std::chrono::steady_clock::now(), std::chrono::steady_clock::now(), 40 };

            auto tick = [](std::size_t done, std::size_t total, void* u) {
                auto* b = static_cast<Bar*>(u);
                using namespace std::chrono;
                auto now = steady_clock::now();
                if (done < total && now - b->last < 50ms) return; // throttle to ~20 Hz
                double frac = total ? (double)done / (double)total : 0.0;
                double elapsed = duration_cast<duration<double>>(now - b->start).count();
                double eta = (frac > 0.0) ? elapsed * (1.0 - frac) / frac : 0.0;
                int filled = (int)std::lround(frac * b->width);
                std::cerr << "\r[";
                for (int i = 0; i < b->width; ++i) std::cerr << (i < filled ? "█" : " ");
                std::cerr << "] " << std::setw(3) << (int)std::round(frac * 100.0) << "%  "
                                  << "elapsed " << std::fixed << std::setprecision(1) << elapsed << "s  "
                                  << "eta " << std::fixed << std::setprecision(1) << eta << "s" << std::flush;
                b->last = now;
                if (done == total) std::cerr << "\n";
            };

            piracer::Progress prog;
            prog.tick = tick;
            prog.user = &bar;
            pi = piracer::compute_pi_base_with_progress(digits, base, &prog);
        } else {
            pi = piracer::compute_pi_base(digits, base);
        }

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
                std::cerr << "Wrote " << digits << " " << (base == 16 ? "hex" : "decimal") << " digits to '" << out << "'\n";
            std::cerr << "Elapsed: " << dt.count() << " s\n";
            
            // Calculate and log ns/digit metric
            double ns_per_digit = (dt.count() * 1e9) / digits;
            std::cerr << "Performance: " << std::fixed << std::setprecision(3) << ns_per_digit << " ns/digit\n";
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n"
                  << "Tip: run with '--help' for usage.\n";
        return 2;
    }
}
