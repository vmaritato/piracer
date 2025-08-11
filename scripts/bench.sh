#!/usr/bin/env bash
set -euo pipefail

# Simple repeatable benches. Usage ./script/bench.sh 1e5 5e5 1e6

BUILD_DIR=${BUILD_DIR:-build}
BIN=${BIN:-$BUILD_DIR/piracer}
REPEATS=${REPEATS:-3}

if [ ! -x "$BIN" ]; then
    echo "Build first: cmake -S . -B $BUILD_DIR -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build $BUILD_DIR -j" >&2
    exit 1
fi

echo "digits,run,seconds" > bench.csv

for d in "$@"; do
    for r in $(seq 1 $REPEATS); do
        START=$(date +%s.%N)
        $BIN --digits $d --out /dev/null >/dev/null
        END=$(date +%s.%N)
        python3 - <<PY >> bench.csv
import sys
s = float("$START"); e = float("$END");
print(f"{int(float('$d'))},{int('$r')},{e-s:.6f}")
PY
    done
done

echo "Done. Results in bench.csv"