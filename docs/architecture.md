# PiRacer — Architecture

## Modules
- **piracer-core** (static lib)
  - `alg/pi/` — `chudnovsky.cpp` (precision, MPFR assembly)  
  - `alg/pi/bsplit.cpp` — binary-splitting engine (term tree + leaf math)  
  - `core/format.cpp` — MPFR → fixed decimal string  
  - `core/selftest.cpp` — MPFR const_pi comparison  
  - `progress.hpp` — lightweight progress callback
- **CLI**
  - `cli/main.cpp` — parsing, banner/help, self-test, timings, progress rendering

## Data Flow (Chudnovsky)
```
CLI → parse → compute_pi[_with_progress](digits)
↘ (Progress::tick) → CLI bar/ETA
compute_pi → bsplit_chudnovsky(0..n) → (P,Q,T)
→ MPFR: sqrt(10005), π = (426880·sqrt·Q)/|T|
→ format fixed "3.<N digits>"
```

## Key Design Choices
- **Integer math** in bsplit (GMP), **FP only at the end** (MPFR).
- **Pure functions** in core, side-effects (I/O/log) solo nel CLI.
- Progress via **callback** (no globals).
- Ready for **backend swap** (NTT/CRT) mantenendo API stabili.
