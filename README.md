# PiRacer — Digits at full throttle 🏁🥧

Fast, reproducible π digits engine. Clean C++ architecture. Honest benchmarks.

**Baseline v0.1**: Chudnovsky + binary-splitting using GMP/MPFR. Portable; no GPU.

## Quickstart

```bash
# macOS: brew install gmp mpfr cmake ninja
# Ubuntu: sudo apt-get update && sudo apt-get install -y libgmp-dev libmpfr-dev cmake ninja-build

git clone https://github.com/vmaritato/piracer && cd piracer
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/piracer --digits 100000 --out pi.txt
```

## Roadmap

-

## Why another π engine?

- **Speed that scales** with engineering (not just math tricks)
- **Reproducible** benches (CSV + CI)
- **Readable** architecture (clean modules)

## License

[MIT](LICENSE)

---

### Notes

- This baseline computes π to `--digits N` and writes a fixed-point decimal string (`3.<N digits>`). Tested with Linux/macOS package names above.
- For Windows, plan to add vcpkg in a later commit: `vcpkg install gmp mpfr` and hook toolchain.
- Later sprints will move `bsplit` into `src/alg/pi/bsplit.hpp` etc.; for now it's bundled to keep bootstrap simple.
