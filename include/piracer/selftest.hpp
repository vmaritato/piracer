#pragma once
#include <cstddef>
#include <string>

namespace piracer {
    // Runs a correctness self-test by comparing our Chudnovsky+BSplit output
    // against MPFR's builtin π (mpfr_const_pi) at `digits` decimals.
    // Returns true on match. If `message` is provided, it contains a short verdict.
    bool self_test(std::size_t digits, std::string* message = nullptr);
} // namespace piracer
