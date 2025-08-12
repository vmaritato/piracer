# Contributing to PiRacer

Thank you for your interest in contributing to PiRacer! 🚀

PiRacer is a high-performance π computation engine designed to beat the competition. We welcome contributions from developers, mathematicians, and performance enthusiasts.

## 🎯 Project Goals

- **Performance**: Beat Mini-Pi and compete with y-cruncher
- **Architecture**: Clean, maintainable C++ with modern practices
- **Portability**: Cross-platform support (Linux, macOS, Windows)
- **Innovation**: Novel algorithms and optimizations

## 🚀 Quick Start

### Prerequisites

- **C++17** compiler (GCC 8+, Clang 7+, MSVC 2019+)
- **CMake** 3.18+
- **GMP** 6.2+ and **MPFR** 4.1+
- **Python 3.6+** (for benchmarks)

### Build Setup

```bash
# Clone and build
git clone https://github.com/your-username/piracer.git
cd piracer
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

# Run tests
ctest --test-dir build --output-on-failure

# Quick benchmark
./scripts/bench.py 1000 10000
```

## 🔧 Development Workflow

### 1. Fork and Clone

```bash
# Fork on GitHub, then clone your fork
git clone https://github.com/your-username/piracer.git
cd piracer
git remote add upstream https://github.com/original-username/piracer.git
```

### 2. Create Feature Branch

```bash
git checkout -b feature/amazing-optimization
# or
git checkout -b fix/bug-description
```

### 3. Make Changes

- **Code Style**: Follow existing patterns
- **Testing**: Add tests for new features
- **Documentation**: Update docs for API changes
- **Performance**: Benchmark impact of changes

### 4. Test Your Changes

```bash
# Build and test
cmake --build build -j
ctest --test-dir build --output-on-failure

# Run benchmarks
./scripts/bench.py 1000 10000
./scripts/bench_vs_minipi.py 1000 10000

# Check formatting
# (We'll add clang-format later)
```

### 5. Commit and Push

```bash
git add .
git commit -m "feat: add amazing optimization for 2x speedup"
git push origin feature/amazing-optimization
```

### 6. Create Pull Request

- **Title**: Clear, descriptive (e.g., "feat: implement AVX-512 optimizations")
- **Description**: Explain what, why, and how
- **Performance**: Include benchmark results if applicable
- **Tests**: Confirm all tests pass

## 📝 Code Standards

### C++ Guidelines

- **C++17**: Use modern C++ features
- **Naming**: `snake_case` for functions/variables, `PascalCase` for classes
- **Headers**: Include guards with `#pragma once`
- **Memory**: Prefer RAII, avoid raw pointers
- **Exceptions**: Use exceptions for error handling

### File Organization

```
include/piracer/     # Public headers
src/core/           # Core functionality
src/alg/pi/         # π algorithms
src/cli/            # Command-line interface
tests/              # Unit tests
scripts/            # Build and benchmark scripts
docs/               # Documentation
```

### Example Code

```cpp
namespace piracer {

    class OptimizedAlgorithm {
    public:
        explicit OptimizedAlgorithm(size_t precision);

        // Compute π with given precision
        std::string compute_pi(size_t digits);

    private:
        size_t precision_;
        void validate_input(size_t digits);
    };

} // namespace piracer
```

## 🧪 Testing Guidelines

### Unit Tests

- **Coverage**: Aim for >90% code coverage
- **Naming**: `test_<function>_<scenario>`
- **Isolation**: Each test should be independent
- **Assertions**: Use clear, descriptive assertions

### Performance Tests

```cpp
TEST_CASE("chudnovsky performance") {
    auto start = std::chrono::high_resolution_clock::now();
    auto result = compute_pi_chudnovsky(10000);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    REQUIRE(duration.count() < 10000); // < 10ms for 10k digits
}
```

### Benchmark Validation

```bash
# Before changes
./scripts/bench.py 1000 10000 > before.csv

# After changes
./scripts/bench.py 1000 10000 > after.csv

# Compare results
diff before.csv after.csv
```

## 📊 Performance Contributions

### Optimization Areas

1. **Algorithm**: New π computation methods
2. **SIMD**: Vectorized operations (AVX, SSE, NEON)
3. **Memory**: Efficient allocation and cache usage
4. **Parallelism**: Multi-threading and GPU acceleration
5. **I/O**: Fast output formatting

### Benchmarking

```bash
# Compare with Mini-Pi
./scripts/bench_vs_minipi.py 1000 10000 100000

# Extreme tests
./scripts/bench_extreme.py --max-digits 1000000

# Memory profiling
valgrind --tool=massif ./build/piracer --digits 100000
```

### Performance Targets

- **1000 digits**: < 100 ns/digit
- **10000 digits**: < 80 ns/digit
- **100000 digits**: < 150 ns/digit
- **1M digits**: < 200 ns/digit

## 🐛 Bug Reports

### Before Reporting

1. **Search**: Check existing issues
2. **Reproduce**: Confirm on latest commit
3. **Isolate**: Minimal test case
4. **Environment**: OS, compiler, dependencies

### Bug Report Template

```markdown
**Description**
Clear description of the bug

**Steps to Reproduce**

1. Build with `cmake -S . -B build`
2. Run `./build/piracer --digits 1000`
3. Observe crash

**Expected Behavior**
Should output 1000 digits of π

**Actual Behavior**
Segmentation fault

**Environment**

- OS: Ubuntu 22.04
- Compiler: GCC 11.2
- GMP: 6.2.1
- MPFR: 4.1.0

**Additional Context**
Any other relevant information
```

## 💡 Feature Requests

### Before Requesting

1. **Research**: Check if it's already planned
2. **Use Case**: Clear explanation of need
3. **Impact**: Performance or usability benefit
4. **Complexity**: Rough implementation effort

### Feature Request Template

```markdown
**Feature Description**
Brief description of the feature

**Use Case**
Why this feature is needed

**Proposed Implementation**
Rough approach (optional)

**Alternatives Considered**
Other solutions explored

**Additional Context**
Any other relevant information
```

## 🏗️ Architecture Contributions

### Current Architecture

- **Core Library**: `piracer-core` static library
- **CLI Interface**: Command-line application
- **Algorithm Modules**: Pluggable π computation
- **Memory Management**: Custom allocators
- **Threading**: Thread pool for parallelism

### Extension Points

1. **New Algorithms**: Implement `Algorithm` interface
2. **Backend Systems**: NTT, FFT, or custom multiplication
3. **Output Formats**: Binary, base-N, or custom encoding
4. **Progress Reporting**: Custom progress callbacks

## 📚 Documentation

### Code Documentation

- **Headers**: Document public APIs
- **Implementation**: Explain complex algorithms
- **Examples**: Provide usage examples
- **Performance**: Note performance characteristics

### User Documentation

- **README**: Quick start and examples
- **Architecture**: System design and modules
- **Benchmarks**: Performance data and comparisons
- **Roadmap**: Development plans and progress

## 🤝 Community

### Communication

- **Issues**: GitHub Issues for bugs and features
- **Discussions**: GitHub Discussions for questions
- **Code Review**: Constructive feedback on PRs
- **Contributions**: Recognition for all contributors

### Recognition

- **Contributors**: Listed in README
- **Performance**: Credit for speed improvements
- **Documentation**: Acknowledgment for docs
- **Testing**: Recognition for test coverage

## 🚨 Security

### Reporting Security Issues

- **Private**: Email security@yourdomain.com
- **Timeline**: 90-day disclosure policy
- **Credit**: Recognition for responsible disclosure
- **Coordination**: Work with maintainers

## 📄 License

By contributing to PiRacer, you agree that your contributions will be licensed under the same license as the project.

---

**Thank you for contributing to PiRacer!** 🥧

Your contributions help make π computation faster, more accessible, and more innovative for everyone.
