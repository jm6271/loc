#!/usr/bin/env python3
import subprocess
import sys
import time
import statistics

def benchmark_cli(cli_path, args, warmup_runs=10, benchmark_runs=100):
    """
    Benchmark a CLI application.
    
    Args:
        cli_path: Path to the CLI executable
        args: Arguments to pass to the CLI
        warmup_runs: Number of warm-up runs (excluded from results)
        benchmark_runs: Number of timed benchmark runs
    """
    total_runs = warmup_runs + benchmark_runs
    times = []
    
    print(f"Benchmarking: {cli_path} {' '.join(args)}")
    print(f"Running {warmup_runs} warm-up runs...")
    
    for i in range(total_runs):
        if i == warmup_runs:
            print(f"Starting {benchmark_runs} benchmark runs...")
        
        start = time.perf_counter()
        result = subprocess.run([cli_path] + args, 
                              capture_output=True, 
                              text=True)
        elapsed = time.perf_counter() - start
        
        # Only record times after warm-up
        if i >= warmup_runs:
            times.append(elapsed)
        
        # Optional: Check for errors
        if result.returncode != 0:
            print(f"Warning: Run {i+1} exited with code {result.returncode}")
    
    # Calculate statistics
    avg_time = statistics.mean(times)
    min_time = min(times)
    max_time = max(times)
    median_time = statistics.median(times)
    stdev = statistics.stdev(times) if len(times) > 1 else 0
    
    # Print results
    print("\n" + "="*50)
    print("BENCHMARK RESULTS")
    print("="*50)
    print(f"Average time: {avg_time*1000:.2f} ms")
    print(f"Median time:  {median_time*1000:.2f} ms")
    print(f"Shortest:     {min_time*1000:.2f} ms")
    print(f"Longest:      {max_time*1000:.2f} ms")
    print(f"Std dev:      {stdev*1000:.2f} ms")
    print("="*50)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python benchmark.py <cli_executable> [args...]")
        sys.exit(1)
    
    cli_executable = sys.argv[1]
    cli_args = sys.argv[2:]
    
    benchmark_cli(cli_executable, cli_args)
