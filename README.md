# RISC-V HPC Benchmarking

## Overview

This repository documents performance benchmarking experiments conducted on a RISC-V Linux system. The primary objective is to investigate peak memory bandwidth and floating-point performance characteristics of RISC-V architectures through systematic empirical analysis. The benchmarks included herein provide insight into memory hierarchy behaviour, computational throughput, and parallel scalability on emerging RISC-V platforms.

## System Overview

**Note:** Users should document their specific system configuration here.

Expected information includes:
- Processor model and microarchitecture
- Core count and thread configuration
- Cache hierarchy (L1, L2, L3 sizes and associativity)
- Memory configuration (capacity, type, channels)
- Operating system version and kernel
- Compiler version (GCC, LLVM/Clang)

## Benchmarks Used

### STREAM Benchmark

The STREAM benchmark is a synthetic benchmark designed to measure sustainable memory bandwidth. It performs four vector operations (Copy, Scale, Add, Triad) on large arrays to stress the memory subsystem. The Triad operation is typically considered the most representative of real application behaviour.

### NAS Parallel Benchmarks (NPB)

The NAS Parallel Benchmarks are a suite of kernels and pseudo-applications derived from computational fluid dynamics applications. They provide a standardised method for evaluating parallel computer performance across various problem sizes (classes A through F).

Key kernels include:
- **EP (Embarrassingly Parallel):** Tests floating-point performance with minimal communication
- **CG (Conjugate Gradient):** Tests irregular memory access and communication patterns
- **FT (Fourier Transform):** Tests long-distance communication patterns
- **MG (Multi-Grid):** Tests both short and long-distance communication
- **IS (Integer Sort):** Tests integer computation and communication

### OpenMP Microbenchmarks

Simple parallel kernels implemented using OpenMP directives to evaluate thread scaling and overhead on basic operations such as vector addition and matrix multiplication.

## Methodology

### Compilation

All benchmarks are compiled with optimisation flags appropriate for performance measurement. The primary flags used are:

- `-O3`: Aggressive optimisation including loop transformations and vectorisation
- `-march=native` or `-march=rv64gc`: Target-specific instruction generation
- `-fopenmp`: Enable OpenMP parallel regions

Detailed compiler flag explanations are provided in `tools/compiler_flags.md`.

### Execution

Benchmarks are executed multiple times to ensure statistical validity. For memory bandwidth tests, array sizes are chosen to exceed last-level cache capacity. For compute-intensive kernels, problem sizes are selected to provide sufficient runtime for accurate measurement while avoiding excessive execution time.

### Measurement

Performance metrics are obtained through:
- Wall-clock timing using `clock_gettime(CLOCK_MONOTONIC, ...)`
- Calculation of derived metrics (bandwidth in GB/s, FLOPS)
- Analysis of scaling behaviour with thread count

### Interpretation

Results are interpreted in the context of theoretical hardware limits:

- **Memory-bound workloads:** Performance limited by memory bandwidth. STREAM Triad typically achieves 70-90% of theoretical peak bandwidth.
- **Compute-bound workloads:** Performance limited by floating-point throughput. Achievable FLOPS depend on instruction mix, vectorisation, and pipeline utilisation.

The Roofline performance model provides a framework for understanding whether a given kernel is memory-bound or compute-bound based on its arithmetic intensity (FLOPS per byte of memory traffic).

## Observations

**Note:** Users should document their experimental findings here.

This section should include:
- Measured memory bandwidth from STREAM
- Comparison with theoretical peak bandwidth
- Observed scaling behaviour with OpenMP threads
- Performance characteristics of NPB kernels
- Identification of bottlenecks (memory vs compute)
- Compiler-specific observations

## Repository Structure

```
riscv-hpc-benchmarking/
│
├── README.md                      # This file
│
├── stream/                        # STREAM memory bandwidth benchmark
│   ├── stream.c                   # Standard STREAM implementation
│   ├── Makefile                   # Build configuration
│   ├── run.sh                     # Execution script
│   └── results.md                 # Results and interpretation
│
├── nas-benchmarks/                # NAS Parallel Benchmarks documentation
│   ├── overview.md                # Kernel descriptions
│   ├── run_notes.md               # Compilation and execution guide
│   └── config_examples.md         # Example make.def configurations
│
├── openmp-examples/               # Simple OpenMP microbenchmarks
│   ├── vector_add.c               # Parallel vector addition
│   ├── matmul.c                   # Parallel matrix multiplication
│   └── Makefile                   # Build configuration
│
├── analysis/                      # Performance analysis documentation
│   ├── bandwidth_analysis.md      # Memory hierarchy analysis
│   ├── flops_analysis.md          # Compute performance analysis
│   └── roofline_notes.md          # Roofline model explanation
│
└── tools/                         # Supporting documentation
    └── compiler_flags.md          # Compiler optimisation flags
```

## Future Work

### MPI Scaling

Extension to distributed-memory parallelism using MPI would enable evaluation of multi-node scaling behaviour. This is particularly relevant for the NAS Parallel Benchmarks, which include MPI implementations.

### Hybrid Parallelism

Investigation of hybrid MPI+OpenMP parallelism would provide insight into optimal process/thread decomposition on NUMA systems and clusters.

### Vectorisation Analysis

Detailed analysis of SIMD vectorisation effectiveness on RISC-V vector extension (RVV) implementations, including comparison of auto-vectorisation versus explicit vector intrinsics.

### Energy Efficiency

Measurement of energy consumption and calculation of performance-per-watt metrics to evaluate energy efficiency of RISC-V systems relative to established architectures.

### Extended Kernel Coverage

Addition of other relevant benchmarks such as HPCG (High Performance Conjugate Gradient), LINPACK, and domain-specific kernels from applications in computational science.

## References

- McCalpin, J. D. (1995). Memory Bandwidth and Machine Balance in Current High Performance Computers. IEEE Computer Society Technical Committee on Computer Architecture (TCCA) Newsletter.
- Bailey, D. H., et al. (1991). The NAS Parallel Benchmarks. International Journal of High Performance Computing Applications, 5(3), 63-73.
- Williams, S., Waterman, A., & Patterson, D. (2009). Roofline: An Insightful Visual Performance Model for Multicore Architectures. Communications of the ACM, 52(4), 65-76.
