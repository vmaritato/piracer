#!/usr/bin/env bash
set -euo pipefail

# Setup script for Mini-Pi to enable comparative benchmarks
# This script clones and builds Mini-Pi for performance comparison

MINIPI_DIR="minipi"
MINIPI_REPO="https://github.com/Mysticial/Mini-Pi.git"

echo "Setting up Mini-Pi for benchmark comparison..."

# Clone Mini-Pi if not already present
if [ ! -d "$MINIPI_DIR" ]; then
    echo "Cloning Mini-Pi from $MINIPI_REPO..."
    git clone "$MINIPI_REPO" "$MINIPI_DIR"
    cd "$MINIPI_DIR"
else
    echo "Mini-Pi directory already exists, updating..."
    cd "$MINIPI_DIR"
    git pull origin master
fi

# Build Mini-Pi
echo "Building Mini-Pi..."
make clean || true

# Try different build approaches for different platforms
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "macOS detected, trying without OpenMP..."
    # macOS doesn't support -fopenmp by default
    g++ -Wall -Wextra -O3 -g -march=native -std=c++17 -o mini-pi mini-pi.cpp || \
    clang++ -Wall -Wextra -O3 -g -march=native -std=c++17 -o mini-pi mini-pi.cpp
else
    # Linux with OpenMP
    make
fi

# Check if build was successful
if [ -f "mini-pi" ]; then
    echo "✅ Mini-Pi built successfully!"
    echo "Executable: $(pwd)/mini-pi"
    echo ""
    echo "You can now run comparative benchmarks:"
    echo "  cd .."
    echo "  ./scripts/bench_vs_minipi.py 1000 10000 100000"
else
    echo "❌ Mini-Pi build failed!"
    echo "Check the build output above for errors."
    exit 1
fi

# Test Mini-Pi with a small computation
echo ""
echo "Testing Mini-Pi with 1000 digits..."
./mini-pi 1000 > /dev/null
if [ $? -eq 0 ]; then
    echo "✅ Mini-Pi test successful!"
else
    echo "❌ Mini-Pi test failed!"
    exit 1
fi

echo ""
echo "Mini-Pi setup complete! 🎯"
echo "Ready to compete with PiRacer!" 