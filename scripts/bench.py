#!/usr/bin/env python3
import argparse, json, os, statistics, subprocess, time

def run_once(bin_path, digits):
    t0 = time.perf_counter()
    subprocess.run([bin_path, "--digits", str(digits), "--out", os.devnull],
                   check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    t1 = time.perf_counter()
    return t1 - t0

def main():
    ap = argparse.ArgumentParser(description="PiRacer benchmark runner")
    ap.add_argument("--bin", default="build/piracer")
    ap.add_argument("--reps", type=int, default=3)
    ap.add_argument("digits", nargs="+", type=int)
    args = ap.parse_args()

    rows = []
    for d in args.digits:
        times = [run_once(args.bin, d) for _ in range(args.reps)]
        med = statistics.median(times)
        ns_per_digit = med * 1e9 / d
        rows.append({"digits": d, "median_s": med, "ns_per_digit": ns_per_digit, "all_s": times})

    print("digits,median_s,ns_per_digit")
    for r in rows:
        print(f"{r['digits']},{r['median_s']:.6f},{r['ns_per_digit']:.3f}")

if __name__ == "__main__":
    main() 