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
- ✅ **Checkpoint/resume** with JSON serialization and integrity validation
- ✅ **Progress events** and structured logging with callbacks
- ✅ **Performance metrics** and ETA calculations
- ✅ **CI enhancements**: sanitizer jobs, perf smoke tests

## v0.4 ✅ - Compatibility & CI

- ✅ **CI Enhancements**: Sanitizer jobs, performance testing, security scanning
- ✅ **Windows Build Parity**: Cross-platform dependency handling, vcpkg integration
- ✅ **Docker Optimization**: Multi-stage builds, security hardening, dev environment
- ✅ **Cross-Platform Testing**: Automated testing script, configuration validation

## v0.5 ✅ - Advanced SIMD & GPU

- ✅ **Advanced SIMD**: AVX-512, NEON ARM64, vectorization optimization
- ✅ **GPU Support**: CUDA/OpenCL backend for massive multiplications
- ✅ **Performance Profiling**: Detailed analysis tools and metrics
- ✅ **Benchmark Visualization**: Advanced graphs and performance analysis

## v1.0 (now) - Production & Competition

- 🚧 **y-cruncher Competition**: Match or beat the best π computation engines
- 🚧 **Multi-Platform**: ARM64, x86_64, RISC-V support
- 🚧 **Production Ready**: Enterprise features and stability
- 🚧 **Community Ecosystem**: Active contributor base and extensions
- Docs: algorithms & architecture diagrams
- Release binaries + checksums
