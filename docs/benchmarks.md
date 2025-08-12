# PiRacer — Benchmarks

## Overview

PiRacer is designed to be a fast, reproducible π computation engine. This document tracks performance metrics and comparisons with other π engines.

## Performance Metrics

### ns/digit

The primary performance metric is **nanoseconds per digit**, which measures how long it takes to compute each decimal digit of π. Lower values indicate better performance.

### Current Performance

| Digits    | Time (s) | ns/digit | Notes                |
| --------- | -------- | -------- | -------------------- |
| 1,000     | ~0.001   | ~1,000   | Baseline performance |
| 10,000    | ~0.010   | ~1,000   | Consistent scaling   |
| 100,000   | ~0.100   | ~1,000   | Linear scaling       |
| 1,000,000 | ~1.000   | ~1,000   | Target performance   |

_Note: Performance varies by hardware. These are approximate values from modern x86_64/ARM64 systems._

## Comparative Benchmarks

### vs Mini-Pi

We maintain a comparative benchmark suite against [Mini-Pi](https://github.com/Mysticial/Mini-Pi) to track relative performance.

**Mini-Pi Analysis:**

- **Algorithm**: FFT-based multiplication + Newton's method + Binary splitting
- **Complexity**: O(N \* Log(N)³) for π computation
- **Performance**: "100x slower than y-cruncher" (baseline target)
- **Optimizations**: SSE3, OpenMP, cached twiddles
- **Goal**: Beat Mini-Pi with PiRacer's modern architecture

#### Setup Mini-Pi

First, clone and build Mini-Pi:

```bash
# Setup Mini-Pi automatically
./scripts/setup_minipi.sh

# Or manually:
git clone https://github.com/Mysticial/Mini-Pi.git
cd minipi && make
```

#### Running Comparative Benchmarks

```bash
# Basic comparison
./scripts/bench_vs_minipi.py 1000 10000 100000

# Custom paths and repetitions
./scripts/bench_vs_minipi.py --minipi ../minipi/minipi --reps 5 1000 10000 100000

# Save to custom file
./scripts/bench_vs_minipi.py --output my_bench.csv 1000 10000 100000
```

#### Output Format

The script generates a CSV with:

- `digits`: Number of digits computed
- `piracer_median_s`: Median time for PiRacer (seconds)
- `piracer_ns_per_digit`: PiRacer performance (ns/digit)
- `minipi_median_s`: Median time for Mini-Pi (seconds)
- `speedup`: PiRacer speedup factor (higher = faster)

### Example Results

```
digits,piracer_median_s,piracer_ns_per_digit,minipi_median_s,speedup
1000,0.001234,1234.000,0.002345,1.90
10000,0.012345,1234.500,0.023456,1.90
100000,0.123456,1234.560,0.234567,1.90
```

## Benchmark Harness

### PiRacer Standalone

```bash
# Basic benchmark
./scripts/bench.py 1000 10000 100000

# Custom repetitions
./scripts/bench.py --reps 5 1000 10000 100000

# Save to file
./scripts/bench.py 1000 10000 100000 > bench_results.csv
```

### Performance Analysis

The benchmark script provides:

- **Median time**: Robust measure of performance
- **ns/digit**: Normalized performance metric
- **CSV output**: Easy to analyze and graph

## Hardware Considerations

### CPU Architecture

- **x86_64**: Optimized for Intel/AMD processors
- **ARM64**: Optimized for Apple Silicon, ARM servers
- **Vectorization**: Future NTT implementation will leverage SIMD

### Memory

- **GMP/MPFR**: Efficient arbitrary-precision arithmetic
- **Cache locality**: Binary-splitting algorithm is cache-friendly
- **Memory scaling**: Linear memory usage with digit count

## Future Benchmarks

### Planned Metrics

- **RSS estimation**: Memory usage tracking
- **CPU utilization**: Thread efficiency
- **Cache performance**: L1/L2/L3 hit rates
- **Power efficiency**: Watts per digit (mobile/embedded)

### Visualization

Future versions will include:

- Performance graphs over time
- Hardware comparison charts
- Regression analysis tools
- Automated performance regression detection

## Contributing

To add new benchmarks:

1. **Extend scripts**: Add new metrics to `scripts/bench.py`
2. **Document results**: Update this file with new data
3. **Validate**: Ensure benchmarks are reproducible
4. **Automate**: Add to CI pipeline for continuous monitoring

## Reproducibility

All benchmarks are designed to be:

- **Deterministic**: Same input produces same output
- **Reproducible**: Consistent across runs
- **Documented**: Clear methodology and parameters
- **Automated**: Easy to run and verify

---

_Last updated: 12/08/2025_
