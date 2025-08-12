#include "piracer/chudnovsky.hpp"
#include "piracer/cli_utils.hpp"
#include "piracer/version.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

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
        std::cerr << "PiRacer " << piracer::kVersion << " (baseline GMP/MPFR)\n";
    }

    void print_help(const char* argv0) {
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
            << "  " << me << " -n 1e6 -o pi_1M.txt\n";
    }
} // namespace

int main(int argc, char** argv) {
    try {
        std::size_t digits = 0;
        std::string out;
        bool quiet = false;

        for (int i = 1; i < argc; ++i) {
            std::string a = argv[i];
            if ((a == "--digits" || a == "-n") && i + 1 < argc) {
                digits = piracer::parse_digits(argv[++i]);
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
