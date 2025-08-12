# Multi-stage Dockerfile for PiRacer
# Stage 1: Build environment
FROM ubuntu:24.04 AS builder

# Install build dependencies
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    pkg-config \
    libgmp-dev \
    libmpfr-dev \
    python3 \
    valgrind \
    && rm -rf /var/lib/apt/lists/*

# Set build directory
WORKDIR /build

# Copy source code
COPY . .

# Configure and build
RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build -j$(nproc)

# Run tests
RUN ctest --test-dir build --output-on-failure

# Stage 2: Runtime environment
FROM ubuntu:24.04 AS runtime

# Install only runtime dependencies
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    libgmp10 \
    libmpfr6 \
    && rm -rf /var/lib/apt/lists/*

# Create non-root user
RUN useradd -m -u 1000 piracer && \
    mkdir -p /app && \
    chown piracer:piracer /app

# Copy binary from builder
COPY --from=builder --chown=piracer:piracer /build/build/piracer /app/piracer

# Switch to non-root user
USER piracer
WORKDIR /app

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD /app/piracer --digits 100 --base dec > /dev/null || exit 1

# Default command
CMD ["/app/piracer", "--digits", "100000", "--out", "/dev/stdout"]

# Stage 3: Development environment (optional)
FROM builder AS dev

# Install additional development tools
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    gdb \
    strace \
    ltrace \
    && rm -rf /var/lib/apt/lists/*

# Set development working directory
WORKDIR /app

# Copy source code for development
COPY . .

# Expose ports for debugging (if needed)
EXPOSE 22

# Development command
CMD ["/bin/bash"] 