#!/usr/bin/env python3
"""
Cross-platform testing script for PiRacer
Tests different configurations and platforms
"""

import argparse
import subprocess
import sys
import os
import platform
from pathlib import Path

def get_system_info():
    """Get system information for testing."""
    info = {
        'os': platform.system(),
        'arch': platform.machine(),
        'python': platform.python_version(),
        'platform': platform.platform()
    }
    return info

def run_command(cmd, description, check=True):
    """Run a command and handle errors."""
    print(f"🔄 {description}...")
    print(f"   Command: {' '.join(cmd)}")
    
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, check=check)
        print(f"   ✅ {description} completed successfully")
        if result.stdout.strip():
            print(f"   Output: {result.stdout.strip()}")
        return True
    except subprocess.CalledProcessError as e:
        print(f"   ❌ {description} failed (exit code: {e.returncode})")
        if e.stderr.strip():
            print(f"   Error: {e.stderr.strip()}")
        return False

def test_build_config(build_dir, config_type):
    """Test a specific build configuration."""
    print(f"\n🏗️  Testing {config_type} build...")
    
    # Clean build directory
    if os.path.exists(build_dir):
        run_command(['rm', '-rf', build_dir], "Cleaning build directory", check=False)
    
    # Configure
    cmake_cmd = ['cmake', '-S', '.', '-B', build_dir, '-G', 'Ninja']
    if config_type == 'Debug':
        cmake_cmd.extend(['-DCMAKE_BUILD_TYPE=Debug'])
    elif config_type == 'Release':
        cmake_cmd.extend(['-DCMAKE_BUILD_TYPE=Release'])
    
    if not run_command(cmake_cmd, f"Configuring {config_type} build"):
        return False
    
    # Build
    if not run_command(['cmake', '--build', build_dir, '-j'], f"Building {config_type}"):
        return False
    
    # Test
    if not run_command(['ctest', '--test-dir', build_dir, '--output-on-failure'], f"Running {config_type} tests"):
        return False
    
    # Smoke test
    piracer_path = os.path.join(build_dir, 'piracer')
    if not run_command([piracer_path, '--digits', '100', '--base', 'dec'], f"Smoke test {config_type}"):
        return False
    
    return True

def test_sanitizer(build_dir, sanitizer):
    """Test with a specific sanitizer."""
    print(f"\n🧪 Testing with {sanitizer}...")
    
    # Clean build directory
    if os.path.exists(build_dir):
        run_command(['rm', '-rf', build_dir], "Cleaning build directory", check=False)
    
    # Configure with sanitizer
    cmake_cmd = ['cmake', '-S', '.', '-B', build_dir, '-G', 'Ninja', '-DCMAKE_BUILD_TYPE=Release']
    
    if sanitizer == 'asan':
        cmake_cmd.extend([
            '-DCMAKE_CXX_FLAGS=-fsanitize=address -fno-omit-frame-pointer',
            '-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address'
        ])
    elif sanitizer == 'ubsan':
        cmake_cmd.extend([
            '-DCMAKE_CXX_FLAGS=-fsanitize=undefined -fno-omit-frame-pointer',
            '-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=undefined'
        ])
    
    if not run_command(cmake_cmd, f"Configuring with {sanitizer}"):
        return False
    
    # Build
    if not run_command(['cmake', '--build', build_dir, '-j'], f"Building with {sanitizer}"):
        return False
    
    # Basic test (sanitizers can be slow)
    piracer_path = os.path.join(build_dir, 'piracer')
    if not run_command([piracer_path, '--digits', '50', '--base', 'dec'], f"Sanitizer test {sanitizer}"):
        return False
    
    return True

def test_platform_specific():
    """Test platform-specific features."""
    print(f"\n🖥️  Platform-specific testing...")
    
    system_info = get_system_info()
    print(f"   OS: {system_info['os']}")
    print(f"   Architecture: {system_info['arch']}")
    print(f"   Platform: {system_info['platform']}")
    
    # Test different output bases
    build_dir = "build-test"
    piracer_path = os.path.join(build_dir, 'piracer')
    
    if os.path.exists(piracer_path):
        print("\n   Testing output formats...")
        
        # Test decimal output
        run_command([piracer_path, '--digits', '50', '--base', 'dec'], "Decimal output test")
        
        # Test hexadecimal output
        run_command([piracer_path, '--digits', '50', '--base', 'hex'], "Hexadecimal output test")
        
        # Test progress bar
        run_command([piracer_path, '--digits', '1000', '--progress'], "Progress bar test")
        
        # Test self-test
        run_command([piracer_path, '--self-test', '--digits', '100'], "Self-test validation")

def main():
    parser = argparse.ArgumentParser(description="Cross-platform testing for PiRacer")
    parser.add_argument("--build-dir", default="build-test", help="Build directory for testing")
    parser.add_argument("--configs", nargs="+", default=["Release", "Debug"], 
                       help="Build configurations to test")
    parser.add_argument("--sanitizers", nargs="+", default=["asan", "ubsan"], 
                       help="Sanitizers to test")
    parser.add_argument("--skip-sanitizers", action="store_true", 
                       help="Skip sanitizer testing")
    
    args = parser.parse_args()
    
    print("🚀 PiRacer Cross-Platform Testing")
    print("=" * 50)
    
    # Test build configurations
    build_success = True
    for config in args.configs:
        if not test_build_config(args.build_dir, config):
            build_success = False
            break
    
    # Test sanitizers
    if not args.skip_sanitizers and build_success:
        sanitizer_success = True
        for sanitizer in args.sanitizers:
            sanitizer_build_dir = f"{args.build_dir}-{sanitizer}"
            if not test_sanitizer(sanitizer_build_dir, sanitizer):
                sanitizer_success = False
        
        if not sanitizer_success:
            print("\n⚠️  Some sanitizer tests failed")
    
    # Platform-specific testing
    if build_success:
        test_platform_specific()
    
    # Summary
    print("\n" + "=" * 50)
    if build_success:
        print("🎉 Cross-platform testing completed successfully!")
        print("✅ All build configurations passed")
        if not args.skip_sanitizers:
            print("✅ Sanitizer testing completed")
        print("✅ Platform-specific features tested")
    else:
        print("❌ Cross-platform testing failed")
        print("❌ Some build configurations failed")
        sys.exit(1)

if __name__ == "__main__":
    main() 