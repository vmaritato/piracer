FROM ubuntu:24.04

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential cmake ninja-build pkg-config \
    libgmp-dev libmpfr-dev python3 && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .
RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build -j \
    && ctest --test-dir build --output-on-failure

CMD ["./build/piracer", "--digits", "100000", "--out", "/dev/stdout"] 