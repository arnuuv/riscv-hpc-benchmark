# Floating-Point Performance Analysis

## Introduction

Floating-point performance, measured in FLOPS (floating-point operations per second), is a fundamental metric for evaluating computational capability in scientific computing. Understanding whether an application is compute-bound or memory-bound is essential for performance optimisation and system design.

This document provides guidance for analysing floating-point performance on RISC-V systems and distinguishing between compute-bound and memory-bound workloads.

## Floating-Point Operations

### Definition

A floating-point operation (FLOP) is a single arithmetic operation on floating-point numbers:
- Addition or subtraction
- Multiplication
- Division
- Square root
- Fused multiply-add (FMA) counts as 2 FLOPs

### FLOPS Metrics

**FLOPS:** Floating-point operations per second
- **MFLOPS:** 10⁶ FLOPS
- **GFLOPS:** 10⁹ FLOPS
- **TFLOPS:** 10¹² FLOPS
- **PFLOPS:** 10¹⁵ FLOPS

## Theoretical Peak Performance

### Single-Core Performance

Theoretical peak FLOPS for a single core:

```
Peak FLOPS = Frequency × FLOP/cycle
```

**FLOP/cycle depends on:**
- Number of floating-point execution units
- Vector width (scalar vs SIMD/vector)
- Availability of FMA (fused multiply-add) instructions

### RISC-V Floating-Point Capabilities

**Base RISC-V (RV64GC):**
- Scalar floating-point operations (F and D extensions)
- Typically 1-2 FLOP/cycle per core
- No SIMD/vector operations in base ISA

**RISC-V Vector Extension (RVV):**
- Variable-length vector operations
- Vector length depends on implementation (VLEN)
- Potential for significant FLOP/cycle increase
- Example: VLEN=256 bits → 4 double-precision operations per cycle

**Example calculation (scalar):**
- Frequency: 2.0 GHz
- FMA capable: 2 FLOP/cycle
- Peak: 2.0 × 2 = 4.0 GFLOPS per core

**Example calculation (vector, VLEN=256):**
- Frequency: 2.0 GHz
- Vector width: 4 double-precision elements
- FMA capable: 2 FLOP/cycle × 4 elements = 8 FLOP/cycle
- Peak: 2.0 × 8 = 16.0 GFLOPS per core

### Multi-Core Performance

For a system with N cores:

```
Peak System FLOPS = N × Peak FLOPS per core
```

This assumes perfect scaling, which is achievable for compute-bound workloads with no shared resources.

## Arithmetic Intensity

### Definition

Arithmetic intensity (AI) is the ratio of floating-point operations to bytes of memory traffic:

```
AI = FLOP / Bytes transferred
```

Units: FLOP/byte

### Significance

Arithmetic intensity determines whether a kernel is memory-bound or compute-bound:

**Low AI (< 1 FLOP/byte):**
- Memory-bound
- Performance limited by memory bandwidth
- Examples: STREAM, vector operations, sparse matrix-vector multiply

**Moderate AI (1-10 FLOP/byte):**
- Transitional region
- May be memory-bound or compute-bound depending on system balance
- Examples: Some stencil computations, simple iterative solvers

**High AI (> 10 FLOP/byte):**
- Compute-bound
- Performance limited by floating-point throughput
- Examples: Dense matrix multiplication (with blocking), FFT, many NPB kernels

### Calculating Arithmetic Intensity

**Example: Vector addition**
```c
for (i = 0; i < n; i++)
    c[i] = a[i] + b[i];
```
- FLOPs: n (one addition per element)
- Bytes: 3n × sizeof(double) = 24n bytes (2 reads, 1 write)
- AI = n / (24n) = 1/24 ≈ 0.042 FLOP/byte

**Example: Dense matrix multiplication (naive)**
```c
for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
        for (k = 0; k < n; k++)
            C[i][j] += A[i][k] * B[k][j];
```
- FLOPs: 2n³ (multiply and add for each element)
- Bytes (naive): 2n³ × sizeof(double) (assuming no cache reuse)
- AI (naive) = 2n³ / (2n³ × 8) = 0.125 FLOP/byte

**Example: Dense matrix multiplication (with blocking)**
- Same FLOPs: 2n³
- Bytes (with cache reuse): ~3n² × sizeof(double) (loading matrices once)
- AI (blocked) = 2n³ / (3n² × 8) = n/12 FLOP/byte
- For n=1024: AI ≈ 85 FLOP/byte (highly compute-bound)

## Memory-Bound vs Compute-Bound

### Determining the Bottleneck

**Step 1: Calculate theoretical limits**

Memory bandwidth limit:
```
Max FLOPS (memory) = Bandwidth (bytes/s) × AI
```

Compute limit:
```
Max FLOPS (compute) = Peak FLOPS
```

**Step 2: Compare limits**

The smaller value determines the bottleneck:
- If Max FLOPS (memory) < Max FLOPS (compute): **Memory-bound**
- If Max FLOPS (compute) < Max FLOPS (memory): **Compute-bound**

**Step 3: Compare with observed performance**

Efficiency relative to the limiting factor indicates optimisation quality.

### Example Analysis

**System specifications:**
- Peak FLOPS: 64 GFLOPS (16 cores × 4 GFLOPS/core)
- Memory bandwidth: 40 GB/s

**Kernel: Vector addition (AI = 0.042 FLOP/byte)**

Memory limit:
```
Max FLOPS = 40 GB/s × 0.042 FLOP/byte = 1.68 GFLOPS
```

Compute limit:
```
Max FLOPS = 64 GFLOPS
```

**Conclusion:** Memory-bound (1.68 << 64)

**Kernel: Dense matrix multiply, blocked (AI = 85 FLOP/byte for n=1024)**

Memory limit:
```
Max FLOPS = 40 GB/s × 85 FLOP/byte = 3,400 GFLOPS
```

Compute limit:
```
Max FLOPS = 64 GFLOPS
```

**Conclusion:** Compute-bound (64 << 3,400)

## Measuring FLOPS

### Direct Measurement

Count floating-point operations in the code and divide by execution time:

```
FLOPS = (Total FLOPs) / (Execution time)
```

**Example: Matrix multiplication (n×n)**
```c
// 2n³ FLOPs total (n³ multiplies + n³ adds)
double start = get_time();
matmul(A, B, C, n);
double end = get_time();
double gflops = (2.0 * n * n * n) / (end - start) / 1e9;
```

### Hardware Performance Counters

Modern processors provide hardware performance counters that can measure:
- Floating-point instructions retired
- FMA operations
- Vector operations
- Cache misses
- Memory bandwidth utilisation

On RISC-V, access to performance counters depends on implementation and privilege level.

### Benchmark-Based Measurement

Standard benchmarks provide FLOPS measurements:

**NAS Parallel Benchmarks:**
- Report Mop/s (million operations per second)
- EP benchmark is particularly useful for peak FLOPS measurement

**LINPACK/HPL:**
- Solves dense linear system
- Reports GFLOPS
- Widely used for supercomputer rankings (TOP500)

## Optimising Compute-Bound Code

### Algorithmic Optimisations

1. **Increase arithmetic intensity:**
   - Blocking/tiling to improve cache reuse
   - Algorithm selection (e.g., Strassen vs naive matrix multiply)
   - Fusion of operations to reduce memory traffic

2. **Exploit instruction-level parallelism:**
   - Loop unrolling
   - Software pipelining
   - Independent operations for parallel execution units

3. **Vectorisation:**
   - Use SIMD/vector instructions (RVV on RISC-V)
   - Ensure data alignment
   - Avoid dependencies that prevent vectorisation

### Compiler Optimisations

**Essential flags:**
- `-O3`: Aggressive optimisation
- `-march=native` or `-march=rv64gcv`: Enable all available instructions
- `-ffast-math`: Aggressive floating-point optimisations (use with caution)
- `-funroll-loops`: Unroll loops
- `-ftree-vectorize`: Enable auto-vectorisation (default at -O3)

**Verification:**
- `-fopt-info-vec`: Report vectorisation information
- `-fopt-info-missed`: Report missed optimisations

### Parallelisation

**OpenMP:**
```c
#pragma omp parallel for
for (int i = 0; i < n; i++) {
    // compute-intensive work
}
```

**Thread scaling:**
- Compute-bound code should scale linearly with cores
- Measure speedup and parallel efficiency
- Investigate load imbalance if scaling is poor

## Optimising Memory-Bound Code

### Reducing Memory Traffic

1. **Algorithm redesign:**
   - Increase arithmetic intensity
   - Reuse data in cache
   - Avoid redundant loads

2. **Data structure optimisation:**
   - Structure of arrays (SoA) vs array of structures (AoS)
   - Compression techniques
   - Reduced precision where appropriate

3. **Cache blocking:**
   - Tile computations to fit in cache
   - Improve temporal locality
   - Reduce main memory accesses

### Memory Access Patterns

**Sequential access:**
- Enables hardware prefetching
- Maximises bandwidth utilisation
- Preferred for memory-bound code

**Strided access:**
- Reduces effective bandwidth
- May disable prefetching
- Avoid when possible

**Random access:**
- Worst case for bandwidth
- Prevents prefetching
- Consider data structure reorganisation

## Performance Analysis Workflow

### Step 1: Profile the Application

Identify hotspots:
- Which functions consume most time?
- What is the FLOPS rate?
- What is the memory bandwidth utilisation?

### Step 2: Characterise Each Kernel

For each hotspot:
- Count FLOPs
- Estimate memory traffic
- Calculate arithmetic intensity
- Determine if memory-bound or compute-bound

### Step 3: Compare with Theoretical Limits

- Calculate theoretical peak FLOPS
- Calculate memory bandwidth limit
- Determine expected performance
- Compare with observed performance

### Step 4: Optimise

**If compute-bound and below peak:**
- Improve vectorisation
- Increase instruction-level parallelism
- Check for serialisation or dependencies
- Verify compiler optimisations

**If memory-bound and below bandwidth limit:**
- Improve cache utilisation
- Optimise access patterns
- Enable prefetching
- Check for cache conflicts

### Step 5: Iterate

- Measure after each optimisation
- Document changes and impact
- Verify correctness after optimisations

## NAS Parallel Benchmarks Analysis

### EP (Embarrassingly Parallel)

**Characteristics:**
- High arithmetic intensity
- Minimal memory traffic
- Compute-bound

**Expected behaviour:**
- Near-linear scaling with cores
- Performance close to peak FLOPS
- Minimal memory bandwidth usage

**Analysis:**
- Compare achieved GFLOPS with theoretical peak
- Efficiency should be high (> 80%)
- Useful for measuring peak computational capability

### CG (Conjugate Gradient)

**Characteristics:**
- Sparse matrix operations
- Irregular memory access
- Memory-bound

**Expected behaviour:**
- Limited by memory bandwidth
- Scaling limited by memory subsystem
- Performance depends on cache effectiveness

**Analysis:**
- Compare memory bandwidth with STREAM results
- Arithmetic intensity is low
- Cache behaviour is critical

### FT (Fourier Transform)

**Characteristics:**
- Regular memory access
- Moderate arithmetic intensity
- Communication-intensive (in parallel)

**Expected behaviour:**
- May be memory-bound or compute-bound
- Depends on problem size and cache capacity
- All-to-all communication affects parallel scaling

**Analysis:**
- Arithmetic intensity depends on FFT size
- Cache blocking can improve performance
- Compare with theoretical limits for both memory and compute

## Practical Recommendations

### Measurement Best Practices

1. **Warm-up runs:** Execute kernel once before timing
2. **Multiple iterations:** Run 5-10 times, report best or median
3. **Statistical analysis:** Calculate mean, standard deviation, confidence intervals
4. **System state:** Disable frequency scaling, minimise background processes
5. **Verification:** Always verify correctness of results

### Documentation

For each benchmark, record:
- System specifications (processor, memory, cache)
- Compiler version and flags
- Thread count and affinity settings
- Problem size
- Execution time
- Achieved FLOPS
- Comparison with theoretical peak
- Identification of bottleneck (memory or compute)

### Comparison Across Systems

When comparing RISC-V with other architectures:

**Normalise by:**
- Frequency (FLOPS/cycle)
- Core count (FLOPS per core)
- Power consumption (FLOPS/watt)
- Cost (FLOPS/dollar)

**Consider:**
- ISA differences (vector extensions, FMA availability)
- Memory subsystem design
- Compiler maturity and optimisation quality
- Application characteristics

## References

- Hennessy, J. L., & Patterson, D. A. (2017). Computer Architecture: A Quantitative Approach (6th ed.). Morgan Kaufmann.
- Williams, S., Waterman, A., & Patterson, D. (2009). Roofline: An Insightful Visual Performance Model for Multicore Architectures. Communications of the ACM, 52(4), 65-76.
- Bailey, D. H., et al. (1991). The NAS Parallel Benchmarks. International Journal of High Performance Computing Applications, 5(3), 63-73.
- Dongarra, J. J., et al. (2003). The LINPACK Benchmark: Past, Present and Future. Concurrency and Computation: Practice and Experience, 15(9), 803-820.
