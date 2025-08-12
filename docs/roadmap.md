# PiRacer — Roadmap

## v0.1 (now)

- ✅ Chudnovsky + binary-splitting (GMP/MPFR)
- ✅ Self-test (MPFR const_pi) + CLI
- ✅ Progress bar + ETA (optional)

## v0.2 (core)

- NTT/CRT integer convolutions (multi-modulus) with Bluestein
- Parallel binary-splitting + convolution
- Thread pinning + allocator arena

## v0.3 (DX)

- Checkpoint/resume; progress events; structured logging
- Bench harness (CSV; ns/digit; RSS est.) + graphs
- CI: sanitizer jobs; perf smoke

## v0.4 (compatibility)

- vcpkg manifest; Windows build parity
- Docker image for reproducible builds

## v1.0

- Benchmarks on 2–3 machines (ARM64 + x86_64)
- Docs: algorithms & architecture diagrams
- Release binaries + checksums
