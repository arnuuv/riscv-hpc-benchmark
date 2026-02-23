# Memory Bandwidth Analysis

## Introduction

Memory bandwidth is a critical performance factor in modern computing systems. Many scientific and engineering applications are memory-bound, meaning their performance is limited by the rate at which data can be transferred between main memory and the processor, rather than by the computational throughput of the processor itself.

This document provides a framework for analysing memory bandwidth on RISC-V systems using the STREAM benchmark and related measurements.

## Memory Hierarchy

### Overview

Modern computer systems employ a hierarchical memory structure to balance speed, capacity, and cost:

```
Registers (fastest, smallest)
    ↓
L1 Cache (split: instruction and data)
    ↓
L2 Cache (typically unified)
    ↓
L3 Cache (shared, if present)
    ↓
Main Memory (DRAM)
    ↓
Storage (slowest, largest)
```

### Typical Characteristics

**Registers:**
- Access time: < 1 cycle
- Capacity: Tens to hundreds of bytes
- Bandwidth: Matches processor frequency

**L1 Cache:**
- Access time: 1-4 cycles
- Capacity: 32-64 KiB per core (typical)
- Bandwidth: Hundreds of GB/s per core

**L2 Cache:**
- Access time: 10-20 cycles
- Capacity: 256 KiB - 1 MiB per core (typical)
- Bandwidth: Tens to hundreds of GB/s

**L3 Cache:**
- Access time: 40-75 cycles
- Capacity: Several MiB to tens of MiB (shared)
- Bandwidth: Tens of GB/s

**Main Memory (DDR4/DDR5):**
- Access time: 100-300 cycles
- Capacity: GB to TB
- Bandwidth: 10-100 GB/s (system-dependent)

### RISC-V Specific Considerations

RISC-V implementations vary widely in cache hierarchy design. When documenting your system:

- Identify cache sizes at each level
- Determine cache line size (typically 64 bytes)
- Identify cache associativity
- Determine whether caches are inclusive or exclusive
- Document coherency protocol (if multi-core)

## STREAM Benchmark Analysis

### The Four Kernels

The STREAM benchmark consists of four vector operations:

**Copy:** `c[i] = a[i]`
- Memory operations: 1 read, 1 write
- Bytes per element: 2 × sizeof(double) = 16 bytes
- Arithmetic intensity: 0 FLOP/byte

**Scale:** `b[i] = scalar × c[i]`
- Memory operations: 1 read, 1 write
- Bytes per element: 2 × sizeof(double) = 16 bytes
- Arithmetic intensity: 1 FLOP / 16 bytes = 0.0625 FLOP/byte

**Add:** `c[i] = a[i] + b[i]`
- Memory operations: 2 reads, 1 write
- Bytes per element: 3 × sizeof(double) = 24 bytes
- Arithmetic intensity: 1 FLOP / 24 bytes ≈ 0.042 FLOP/byte

**Triad:** `a[i] = b[i] + scalar × c[i]`
- Memory operations: 2 reads, 1 write
- Bytes per element: 3 × sizeof(double) = 24 bytes
- Arithmetic intensity: 2 FLOP / 24 bytes ≈ 0.083 FLOP/byte

### Interpreting STREAM Results

**Bandwidth Calculation:**

The STREAM benchmark reports bandwidth in MB/s. For the Triad operation:

```
Bandwidth (MB/s) = (Bytes transferred) / (Execution time)
                 = (3 × N × sizeof(double)) / time
```

where N is the array size.

**Expected Performance:**

On well-designed systems, STREAM typically achieves:
- 70-90% of theoretical peak memory bandwidth
- Similar bandwidth across all four kernels (memory-bound)
- Good scaling with thread count up to memory bandwidth saturation

**Factors Limiting Bandwidth:**

1. **Memory controller limitations:** Maximum theoretical bandwidth
2. **NUMA effects:** Non-uniform memory access on multi-socket systems
3. **Prefetching effectiveness:** Hardware and software prefetching
4. **TLB misses:** Translation lookaside buffer capacity
5. **Cache pollution:** Interference from other processes

### Array Size Selection

The array size must be chosen carefully:

**Too small:**
- Data fits in cache
- Measures cache bandwidth, not memory bandwidth
- Results not representative of memory-bound applications

**Appropriate size:**
- Exceeds last-level cache capacity by factor of 2-4×
- Ensures data is fetched from main memory
- Typical: 10-100 million elements (80-800 MB per array)

**Too large:**
- May exceed physical memory
- Causes swapping to storage
- Results become meaningless

**Rule of thumb:**
```
Array size > 4 × (Last-level cache size) / sizeof(double)
```

## Theoretical Peak Bandwidth

### Calculation

Theoretical peak memory bandwidth depends on memory technology and configuration:

```
Peak Bandwidth = Memory Frequency × Bus Width × Number of Channels
```

**Example (DDR4-3200):**
- Memory frequency: 3200 MT/s (mega-transfers per second)
- Bus width: 8 bytes (64 bits)
- Channels: 2 (dual-channel)

```
Peak Bandwidth = 3200 × 8 × 2 = 51,200 MB/s ≈ 51.2 GB/s
```

### RISC-V System Documentation

For your RISC-V system, document:
- Memory type (DDR3, DDR4, DDR5, LPDDR4, etc.)
- Memory frequency
- Number of memory channels
- Bus width
- Calculated theoretical peak bandwidth

## Bandwidth Efficiency

### Definition

Bandwidth efficiency is the ratio of observed bandwidth to theoretical peak:

```
Efficiency = (Observed Bandwidth / Theoretical Peak) × 100%
```

### Typical Values

- **Excellent:** 85-95% (highly optimised system)
- **Good:** 70-85% (well-designed system)
- **Acceptable:** 50-70% (typical system)
- **Poor:** < 50% (potential issues)

### Factors Affecting Efficiency

**Hardware factors:**
- Memory controller design
- Cache hierarchy effectiveness
- Prefetching mechanisms
- Memory interleaving

**Software factors:**
- Compiler optimisations
- Data alignment
- Access patterns (sequential vs random)
- Thread placement and affinity

## Parallel Scaling

### Expected Behaviour

When increasing thread count:

1. **Linear scaling region:** Bandwidth increases proportionally with threads
2. **Saturation region:** Bandwidth plateaus as memory bandwidth limit is reached
3. **Degradation region:** Bandwidth may decrease due to contention (rare)

### Analysis

Plot bandwidth vs thread count:

```
Bandwidth (GB/s)
    |
    |         _________ (saturation)
    |        /
    |       /
    |      /  (linear scaling)
    |     /
    |____/________________
         1  2  4  8  16   Threads
```

**Key observations:**
- At what thread count does saturation occur?
- What is the maximum achieved bandwidth?
- Is there any performance degradation at high thread counts?

### NUMA Considerations

On NUMA (Non-Uniform Memory Access) systems:

- Threads should access local memory when possible
- Remote memory access incurs latency and bandwidth penalties
- Thread affinity and memory placement become critical

RISC-V systems with multiple memory controllers or sockets will exhibit NUMA behaviour.

## Memory-Bound vs Compute-Bound

### Arithmetic Intensity

Arithmetic intensity is the ratio of floating-point operations to bytes transferred:

```
Arithmetic Intensity = FLOP / Bytes
```

**Low intensity (< 1 FLOP/byte):** Memory-bound
- STREAM kernels
- Sparse matrix operations
- Many graph algorithms

**High intensity (> 10 FLOP/byte):** Compute-bound
- Dense matrix multiplication (with blocking)
- FFT with sufficient reuse
- Many machine learning kernels

### Determining the Bottleneck

Compare observed performance with theoretical limits:

**Memory bandwidth limit:**
```
Max Performance (FLOP/s) = Bandwidth (bytes/s) × Arithmetic Intensity
```

**Compute limit:**
```
Max Performance (FLOP/s) = Processor Frequency × Cores × FLOP/cycle
```

The minimum of these two limits determines the bottleneck.

## Optimisation Strategies

### For Memory-Bound Code

1. **Reduce memory traffic:**
   - Algorithm redesign to increase arithmetic intensity
   - Data structure optimisation
   - Compression techniques

2. **Improve memory access patterns:**
   - Sequential access (enable prefetching)
   - Avoid strided access
   - Align data structures

3. **Exploit cache hierarchy:**
   - Blocking/tiling to fit working set in cache
   - Loop reordering for better locality
   - Cache-oblivious algorithms

4. **Parallelisation:**
   - Use all available memory bandwidth
   - Proper thread placement
   - NUMA-aware allocation

### Compiler Optimisations

Relevant compiler flags for memory performance:

- `-O3`: Enable aggressive optimisations including prefetching
- `-march=native`: Enable architecture-specific optimisations
- `-funroll-loops`: Unroll loops to reduce overhead
- `-fprefetch-loop-arrays`: Explicit prefetch generation

## Comparison with Other Architectures

When comparing RISC-V memory bandwidth with other architectures:

**Normalise by:**
- Memory technology (DDR4 vs DDR5)
- Number of memory channels
- System cost/power

**Consider:**
- Absolute bandwidth (GB/s)
- Bandwidth per core
- Bandwidth per watt
- Efficiency relative to theoretical peak

## Practical Recommendations

### Measurement Protocol

1. **System preparation:**
   - Disable frequency scaling (set to performance governor)
   - Disable turbo/boost if present
   - Minimise background processes
   - Ensure adequate cooling

2. **Execution:**
   - Run multiple iterations (10+)
   - Report best time (minimum time = maximum bandwidth)
   - Verify results pass correctness checks

3. **Documentation:**
   - Record all system specifications
   - Document compiler version and flags
   - Note environmental conditions
   - Report confidence intervals if performing statistical analysis

### Interpreting Results

When analysing STREAM results:

- Compare Triad bandwidth with theoretical peak
- Check consistency across all four kernels
- Verify scaling behaviour with thread count
- Compare with published results for similar systems
- Identify any anomalies or unexpected behaviour

## References

- McCalpin, J. D. (1995). Memory Bandwidth and Machine Balance in Current High Performance Computers. IEEE TCCA Newsletter.
- Hennessy, J. L., & Patterson, D. A. (2017). Computer Architecture: A Quantitative Approach (6th ed.). Morgan Kaufmann.
- Drepper, U. (2007). What Every Programmer Should Know About Memory. Red Hat, Inc.
