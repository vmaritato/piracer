#!/usr/bin/env python3
"""
Benchmark comparison script: PiRacer vs Mini-Pi
Usage: ./scripts/bench_vs_minipi.py [--minipi PATH] [--piracer PATH] [--digits DIGITS...]
"""

import argparse
import json
import os
import statistics
import subprocess
import time
from pathlib import Path

def run_piracer_once(bin_path, digits, base="dec", threads=1):
    """Run PiRacer once and return execution time."""
    cmd = [bin_path, "--digits", str(digits), "--base", base, "--threads", str(threads), "--out", os.devnull]
    
    t0 = time.perf_counter()
    result = subprocess.run(cmd, capture_output=True, text=True, check=True)
    t1 = time.perf_counter()
    
    # Extract ns/digit from stderr
    ns_per_digit = None
    for line in result.stderr.split('\n'):
        if 'Performance:' in line:
            try:
                ns_per_digit = float(line.split()[1])
                break
            except (IndexError, ValueError):
                pass
    
    return t1 - t0, ns_per_digit

def run_minipi_once(bin_path, digits):
    """Run Mini-Pi once and return execution time."""
    cmd = [bin_path, str(digits)]
    
    t0 = time.perf_counter()
    result = subprocess.run(cmd, capture_output=True, text=True, check=True)
    t1 = time.perf_counter()
    
    return t1 - t0, None  # Mini-Pi doesn't report ns/digit

def benchmark_comparison(piracer_path, minipi_path, digits_list, reps=3):
    """Run comparative benchmarks between PiRacer and Mini-Pi."""
    
    results = []
    
    for digits in digits_list:
        print(f"Benchmarking {digits} digits...")
        
        # PiRacer benchmarks
        piracer_times = []
        piracer_ns_per_digit = []
        
        for run in range(reps):
            time_taken, ns_per_digit = run_piracer_once(piracer_path, digits)
            piracer_times.append(time_taken)
            if ns_per_digit:
                piracer_ns_per_digit.append(ns_per_digit)
            print(f"  PiRacer run {run+1}: {time_taken:.6f}s")
        
        # Mini-Pi benchmarks
        minipi_times = []
        
        for run in range(reps):
            time_taken, _ = run_minipi_once(minipi_path, digits)
            minipi_times.append(time_taken)
            print(f"  Mini-Pi run {run+1}: {time_taken:.6f}s")
        
        # Calculate statistics
        piracer_median = statistics.median(piracer_times)
        minipi_median = statistics.median(minipi_times)
        
        piracer_ns_median = statistics.median(piracer_ns_per_digit) if piracer_ns_per_digit else None
        
        speedup = minipi_median / piracer_median if piracer_median > 0 else float('inf')
        
        results.append({
            'digits': digits,
            'piracer_median_s': piracer_median,
            'piracer_ns_per_digit': piracer_ns_median,
            'minipi_median_s': minipi_median,
            'speedup': speedup,
            'piracer_all_s': piracer_times,
            'minipi_all_s': minipi_times
        })
        
        print(f"  Results: PiRacer {piracer_median:.6f}s, Mini-Pi {minipi_median:.6f}s")
        print(f"  Speedup: {speedup:.2f}x")
        print()
    
    return results

def save_csv(results, output_file):
    """Save results to CSV file."""
    with open(output_file, 'w') as f:
        f.write("digits,piracer_median_s,piracer_ns_per_digit,minipi_median_s,speedup\n")
        for r in results:
            ns_per_digit = r['piracer_ns_per_digit'] if r['piracer_ns_per_digit'] else "N/A"
            f.write(f"{r['digits']},{r['piracer_median_s']:.6f},{ns_per_digit},{r['minipi_median_s']:.6f},{r['speedup']:.2f}\n")

def main():
    parser = argparse.ArgumentParser(description="PiRacer vs Mini-Pi benchmark comparison")
    parser.add_argument("--minipi", default="minipi", help="Path to Mini-Pi executable")
    parser.add_argument("--piracer", default="build/piracer", help="Path to PiRacer executable")
    parser.add_argument("--reps", type=int, default=3, help="Number of repetitions per test")
    parser.add_argument("--output", default="bench_comparison.csv", help="Output CSV file")
    parser.add_argument("digits", nargs="+", type=int, help="Digits to benchmark")
    
    args = parser.parse_args()
    
    # Check if executables exist
    if not Path(args.piracer).exists():
        print(f"Error: PiRacer not found at {args.piracer}")
        return 1
    
    if not Path(args.minipi).exists():
        print(f"Error: Mini-Pi not found at {args.minipi}")
        print("Note: This script expects Mini-Pi to be available. Clone it first:")
        print("  git clone https://github.com/your-minipi-repo")
        print("  cd minipi && make")
        return 1
    
    print(f"PiRacer: {args.piracer}")
    print(f"Mini-Pi: {args.minipi}")
    print(f"Digits: {args.digits}")
    print(f"Repetitions: {args.reps}")
    print()
    
    # Run benchmarks
    results = benchmark_comparison(args.piracer, args.minipi, args.digits, args.reps)
    
    # Save results
    save_csv(results, args.output)
    print(f"Results saved to {args.output}")
    
    # Summary
    print("\nSummary:")
    for r in results:
        print(f"  {r['digits']} digits: {r['speedup']:.2f}x speedup")
    
    return 0

if __name__ == "__main__":
    exit(main()) 