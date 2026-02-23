#!/bin/bash

# STREAM Benchmark Execution Script
# This script demonstrates how to run the STREAM benchmark with various configurations

echo "Building STREAM benchmark..."
make clean
make

echo ""
echo "=========================================="
echo "Running STREAM (serial version)"
echo "=========================================="
./stream

echo ""
echo "Building STREAM with OpenMP support..."
make clean
make openmp

echo ""
echo "=========================================="
echo "Running STREAM with OpenMP (1 thread)"
echo "=========================================="
export OMP_NUM_THREADS=1
./stream

echo ""
echo "=========================================="
echo "Running STREAM with OpenMP (2 threads)"
echo "=========================================="
export OMP_NUM_THREADS=2
./stream

echo ""
echo "=========================================="
echo "Running STREAM with OpenMP (4 threads)"
echo "=========================================="
export OMP_NUM_THREADS=4
./stream

echo ""
echo "=========================================="
echo "Running STREAM with OpenMP (all available threads)"
echo "=========================================="
unset OMP_NUM_THREADS
./stream

echo ""
echo "Benchmark runs completed."
echo "Results should be documented in results.md"
