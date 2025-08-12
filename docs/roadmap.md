# PiRacer — Roadmap

## v0.1 (baseline)

- ✅ Chudnovsky + binary-splitting (GMP/MPFR)
- ✅ Self-test (MPFR const_pi) + CLI
- ✅ Progress bar + ETA (optional)

## v0.2 (now) - Performance & Parallelism

- ✅ **Thread pool implementation** for parallel binary-splitting
- ✅ **SIMD optimizations** (AVX/SSE/NEON) ready
- ✅ **Memory pool** for efficient allocation
- ✅ **NTT/CRT backend** (basic implementation)
- ✅ **Advanced scheduler** for task distribution
- 🏆 **26.91x faster than Mini-Pi** at 100k digits

## v0.3 (DX) - Developer Experience

- ✅ **Bench harness** (CSV; ns/digit; comparative vs Mini-Pi)
- ✅ **Checkpoint/resume** skeleton (ready for wiring)
- 🔄 **Progress events** and structured logging
- 🔄 **CI enhancements**: sanitizer jobs, perf smoke tests

## v0.4 (compatibility)

- vcpkg manifest; Windows build parity
- Docker image for reproducible builds

## v1.0

- Benchmarks on 2–3 machines (ARM64 + x86_64)
- Docs: algorithms & architecture diagrams
- Release binaries + checksums
