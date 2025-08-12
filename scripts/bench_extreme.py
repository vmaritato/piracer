#!/usr/bin/env python3
"""
Extreme benchmark script for PiRacer
Tests performance at very high digit counts
"""

import argparse
import subprocess
import time
import statistics
import sys
from pathlib import Path

def run_piracer_extreme(bin_path, digits, base="dec", threads=1):
    """Run PiRacer with extreme digit counts."""
    cmd = [bin_path, "--digits", str(digits), "--base", base, "--threads", str(threads)]
    
    print(f"  Computing {digits:,} digits with {threads} threads...")
    
    t0 = time.perf_counter()
    result = subprocess.run(cmd, capture_output=True, text=True, check=True)
    t1 = time.perf_counter()
    
    # Extract performance metrics
    elapsed = t1 - t0
    ns_per_digit = (elapsed * 1e9) / digits
    
    # Extract output length for verification
    output_lines = result.stdout.strip().split('\n')
    actual_digits = len(output_lines[-1]) - 2  # Remove "3." prefix
    
    return elapsed, ns_per_digit, actual_digits

def main():
    parser = argparse.ArgumentParser(description="Extreme PiRacer benchmark")
    parser.add_argument("--bin", default="build/piracer", help="PiRacer executable path")
    parser.add_argument("--max-digits", type=int, default=1000000, help="Maximum digits to test")
    parser.add_argument("--threads", type=int, default=1, help="Number of threads")
    parser.add_argument("--base", choices=["dec", "hex"], default="dec", help="Output base")
    
    args = parser.parse_args()
    
    if not Path(args.bin).exists():
        print(f"Error: PiRacer not found at {args.bin}")
        return 1
    
    print(f"🚀 Extreme PiRacer Benchmark")
    print(f"Binary: {args.bin}")
    print(f"Threads: {args.threads}")
    print(f"Base: {args.base}")
    print(f"Max digits: {args.max_digits:,}")
    print()
    
    # Test progression: 10k, 100k, 1M
    test_digits = [10000, 100000, 1000000]
    test_digits = [d for d in test_digits if d <= args.max_digits]
    
    results = []
    
    for digits in test_digits:
        print(f"🎯 Testing {digits:,} digits...")
        
        try:
            elapsed, ns_per_digit, actual_digits = run_piracer_extreme(
                args.bin, digits, args.base, args.threads
            )
            
            print(f"  ✅ Completed in {elapsed:.3f}s")
            print(f"  📊 Performance: {ns_per_digit:.1f} ns/digit")
            print(f"  🔢 Output: {actual_digits:,} digits")
            print()
            
            results.append({
                'digits': digits,
                'elapsed': elapsed,
                'ns_per_digit': ns_per_digit,
                'actual_digits': actual_digits
            })
            
        except subprocess.CalledProcessError as e:
            print(f"  ❌ Failed: {e}")
            print(f"  stderr: {e.stderr}")
            break
        except Exception as e:
            print(f"  ❌ Error: {e}")
            break
    
    # Summary
    if results:
        print("🏆 Benchmark Summary")
        print("=" * 50)
        print(f"{'Digits':>10} {'Time(s)':>10} {'ns/digit':>12} {'Output':>10}")
        print("-" * 50)
        
        for r in results:
            print(f"{r['digits']:>10,} {r['elapsed']:>10.3f} {r['ns_per_digit']:>12.1f} {r['actual_digits']:>10,}")
        
        print()
        
        # Scaling analysis
        if len(results) > 1:
            print("📈 Scaling Analysis")
            print("-" * 20)
            
            for i in range(1, len(results)):
                prev = results[i-1]
                curr = results[i]
                
                digit_ratio = curr['digits'] / prev['digits']
                time_ratio = curr['elapsed'] / prev['elapsed']
                perf_ratio = curr['ns_per_digit'] / prev['ns_per_digit']
                
                print(f"  {prev['digits']:,} → {curr['digits']:,} digits:")
                print(f"    Time scaling: {time_ratio:.2f}x (ideal: {digit_ratio:.1f}x)")
                print(f"    Perf scaling: {perf_ratio:.2f}x (ideal: 1.0x)")
                print()
    
    return 0

if __name__ == "__main__":
    exit(main()) 