# Roofline Performance Model

## Introduction

The Roofline model is a visually intuitive performance model that provides insight into the performance characteristics of computational kernels on a given hardware platform. It combines two fundamental hardware limitations—peak floating-point performance and memory bandwidth—into a single framework for understanding performance bottlenecks.

Developed by Williams, Waterman, and Patterson in 2009, the Roofline model has become a standard tool for performance analysis in high-performance computing.

## Conceptual Foundation

### The Two Limits

Every computational kernel on a given system is limited by one of two factors:

1. **Peak floating-point performance (compute bound)**
   - Maximum FLOPS the processor can achieve
   - Depends on frequency, number of cores, and FLOP/cycle

2. **Memory bandwidth (memory bound)**
   - Maximum rate of data transfer from memory
   - Depends on memory technology and configuration

The Roofline model visualises these limits and helps identify which factor limits performance for a given kernel.

## The Roofline Plot

### Axes

**X-axis: Arithmetic Intensity (AI)**
- Units: FLOP/byte
- Logarithmic scale
- Represents the ratio of computation to memory traffic
- AI = (Total FLOPs) / (Total bytes transferred)

**Y-axis: Performance**
- Units: FLOPS (typically GFLOPS)
- Logarithmic scale
- Represents achieved or achievable performance

### The Roofline

The roofline itself consists of two regions:

**Memory-bound region (left side):**
- Performance increases linearly with arithmetic intensity
- Slope = Memory bandwidth
- Performance = Bandwidth × AI

**Compute-bound region (right side):**
- Performance is constant (horizontal line)
- Limited by peak FLOPS
- Performance = Peak FLOPS

**Transition point (ridge point):**
- Where memory-bound and compute-bound regions meet
- AI at ridge = Peak FLOPS / Bandwidth
- Kernels with AI < ridge are memory-bound
- Kernels with AI > ridge are compute-bound

### Mathematical Formulation

The roofline function is:

```
Attainable FLOPS = min(Peak FLOPS, Bandwidth × AI)
```

This piecewise function creates the characteristic "roofline" shape.

## Constructing a Roofline

### Step 1: Determine System Parameters

**Peak FLOPS:**
```
Peak FLOPS = Cores × Frequency × FLOP/cycle
```

Example:
- 16 cores
- 2.0 GHz frequency
- 4 FLOP/cycle (with FMA)
- Peak = 16 × 2.0 × 4 = 128 GFLOPS

**Memory Bandwidth:**

Measure using STREAM benchmark (Triad operation):
- Example: 40 GB/s

### Step 2: Calculate Ridge Point

```
Ridge AI = Peak FLOPS / Bandwidth
```

Example:
```
Ridge AI = 128 GFLOPS / 40 GB/s = 3.2 FLOP/byte
```

Kernels with AI < 3.2 are memory-bound; kernels with AI > 3.2 are compute-bound.

### Step 3: Plot the Roofline

**Memory-bound region:**
- For AI from 0.01 to ridge point
- Performance = Bandwidth × AI
- Example: At AI = 1.0, Performance = 40 GFLOPS

**Compute-bound region:**
- For AI from ridge point to 1000
- Performance = Peak FLOPS = 128 GFLOPS

**Conceptual plot:**

```
Performance (GFLOPS)
    |
128 |_________________ (compute bound)
    |               /
 64 |             /
    |           /
 32 |         /
    |       /
 16 |     /
    |   /  (memory bound)
  8 | /
    |/______|______|______|______|
   0.1    1.0    10    100   1000  AI (FLOP/byte)
         Ridge ≈ 3.2
```

## Interpreting the Roofline

### Plotting Kernel Performance

For each kernel:
1. Calculate arithmetic intensity (AI)
2. Measure achieved performance (GFLOPS)
3. Plot point at (AI, GFLOPS)

### Performance Analysis

**Position relative to roofline:**

**On or near the roofline:**
- Excellent performance
- Achieving hardware limits
- Further optimisation difficult

**Below the roofline:**
- Performance gap
- Optimisation opportunity
- Investigate bottlenecks

**Distance from roofline:**
- Vertical distance indicates performance gap
- Large gap suggests significant optimisation potential
- Small gap suggests well-optimised code

### Optimisation Strategies

**For memory-bound kernels (left of ridge):**

**Vertical optimisation (increase performance):**
- Improve memory access patterns
- Enable prefetching
- Reduce cache misses
- Optimise data structures

**Horizontal optimisation (increase AI):**
- Cache blocking to increase data reuse
- Algorithm transformation
- Fusion of operations
- Reduce memory traffic

**For compute-bound kernels (right of ridge):**

**Vertical optimisation only:**
- Improve vectorisation
- Increase instruction-level parallelism
- Better compiler optimisations
- Reduce serialisation

Moving right (increasing AI) does not help compute-bound kernels.

## Example Kernel Analysis

### STREAM Triad

**Kernel:** `a[i] = b[i] + scalar * c[i]`

**Analysis:**
- FLOPs: 2n (multiply + add)
- Bytes: 3n × 8 = 24n (2 reads + 1 write, double precision)
- AI = 2n / 24n = 0.083 FLOP/byte

**On roofline (40 GB/s bandwidth):**
```
Expected performance = 40 × 0.083 = 3.3 GFLOPS
```

**Observation:**
- Very low AI
- Deeply memory-bound
- Performance limited by bandwidth
- Increasing compute power does not help

### Dense Matrix Multiplication (Naive)

**Kernel:** Triple-nested loop, no blocking

**Analysis:**
- FLOPs: 2n³
- Bytes: ~2n³ × 8 (poor cache reuse)
- AI ≈ 0.125 FLOP/byte

**Observation:**
- Low AI
- Memory-bound despite high computational work
- Poor cache utilisation
- Optimisation: blocking

### Dense Matrix Multiplication (Blocked)

**Kernel:** Cache-blocked implementation

**Analysis:**
- FLOPs: 2n³ (same)
- Bytes: ~3n² × 8 (good cache reuse)
- AI = 2n³ / (3n² × 8) = n/12 FLOP/byte
- For n=1024: AI ≈ 85 FLOP/byte

**On roofline (128 GFLOPS peak, 40 GB/s bandwidth):**
```
Ridge = 3.2 FLOP/byte
AI = 85 >> Ridge
Expected performance = 128 GFLOPS (compute-bound)
```

**Observation:**
- High AI
- Compute-bound
- Performance limited by FLOPS, not bandwidth
- Blocking transformed memory-bound to compute-bound

### NAS EP Benchmark

**Characteristics:**
- Minimal memory traffic
- High computational work
- AI > 100 FLOP/byte

**Observation:**
- Deeply compute-bound
- Should achieve near-peak FLOPS
- Excellent for measuring peak performance

### NAS CG Benchmark

**Characteristics:**
- Sparse matrix operations
- Irregular memory access
- AI ≈ 0.2 FLOP/byte

**Observation:**
- Memory-bound
- Performance limited by memory subsystem
- Cache behaviour critical

## Multi-Level Rooflines

### Hierarchical Memory

Modern systems have multiple memory levels:
- L1 cache bandwidth
- L2 cache bandwidth
- L3 cache bandwidth
- Main memory bandwidth

Each level has different bandwidth and can be represented as a separate roofline.

### Cache Rooflines

**L1 Roofline:**
- Highest bandwidth (hundreds of GB/s)
- Highest ridge point
- Represents best-case performance if data fits in L1

**L2 Roofline:**
- Lower bandwidth than L1
- Lower ridge point
- Performance if data fits in L2 but not L1

**L3 Roofline:**
- Lower bandwidth than L2
- Represents performance if data fits in L3

**DRAM Roofline:**
- Lowest bandwidth
- Lowest ridge point
- Worst-case performance

### Interpretation

A kernel's performance depends on which memory level it accesses:
- Small working sets: L1 performance
- Medium working sets: L2/L3 performance
- Large working sets: DRAM performance

Optimisation goal: Move kernel to higher roofline by improving cache locality.

## Limitations of the Roofline Model

### Simplifications

1. **Assumes perfect overlap:** Does not account for inability to overlap computation and memory access
2. **Ignores instruction mix:** Different operations have different costs
3. **Assumes sustained bandwidth:** Peak bandwidth may not be sustainable
4. **Ignores latency:** Only considers bandwidth, not latency
5. **Simplified memory model:** Does not capture NUMA effects, cache conflicts, etc.

### Extensions

**Advanced roofline models address some limitations:**
- Multiple rooflines for different data types (single vs double precision)
- Separate rooflines for different operations (add, multiply, FMA)
- Rooflines for different parallelism levels (scalar, vector, thread)
- Communication rooflines for distributed systems

## Practical Application

### Workflow

1. **Characterise hardware:**
   - Measure peak FLOPS (use EP benchmark or micro-benchmark)
   - Measure memory bandwidth (use STREAM)
   - Calculate ridge point

2. **Analyse kernels:**
   - Identify computational hotspots
   - Calculate arithmetic intensity for each
   - Measure achieved performance

3. **Plot on roofline:**
   - Visualise performance relative to hardware limits
   - Identify memory-bound vs compute-bound kernels

4. **Optimise:**
   - For memory-bound: improve cache utilisation, reduce traffic
   - For compute-bound: improve vectorisation, parallelism
   - Measure and re-plot to track progress

5. **Iterate:**
   - Continue optimisation
   - Document changes and impact

### Tools

**Automated roofline tools:**
- Intel Advisor (x86)
- NVIDIA Nsight (GPU)
- Likwid (various architectures)
- Manual calculation and plotting (always available)

For RISC-V systems, manual calculation may be necessary due to limited tool support.

## RISC-V Specific Considerations

### Scalar vs Vector

**Scalar RISC-V (RV64GC):**
- Lower peak FLOPS
- Lower ridge point
- More kernels are compute-bound

**Vector RISC-V (RV64GCV with RVV):**
- Higher peak FLOPS
- Higher ridge point
- More kernels become memory-bound
- Importance of memory bandwidth increases

### Compiler Maturity

RISC-V compiler optimisation is evolving:
- Auto-vectorisation quality varies
- Manual optimisation may be necessary
- Performance may be below roofline due to compiler limitations
- Document compiler version and flags

### Comparison with Established Architectures

When comparing RISC-V rooflines with x86 or ARM:
- Normalise by frequency and core count
- Consider ISA differences (vector width, FMA availability)
- Account for memory subsystem design
- Recognise compiler maturity differences

## Example: Constructing a RISC-V Roofline

### System Specifications

**Processor:**
- 8 cores
- 1.5 GHz frequency
- Scalar floating-point (no RVV)
- 2 FLOP/cycle per core (with FMA)

**Memory:**
- DDR4-2400
- Dual-channel
- Measured STREAM bandwidth: 25 GB/s

### Calculations

**Peak FLOPS:**
```
Peak = 8 cores × 1.5 GHz × 2 FLOP/cycle = 24 GFLOPS
```

**Ridge point:**
```
Ridge AI = 24 GFLOPS / 25 GB/s = 0.96 FLOP/byte
```

### Interpretation

- Low ridge point (< 1 FLOP/byte)
- Many kernels will be compute-bound
- Memory bandwidth is relatively high compared to compute capability
- System is balanced for memory-intensive workloads

### Plotting Benchmarks

**STREAM Triad (AI = 0.083):**
- Memory-bound
- Expected: 25 × 0.083 = 2.1 GFLOPS
- Achieved: ~2.0 GFLOPS (good)

**Matrix multiply, blocked (AI = 85 for n=1024):**
- Compute-bound
- Expected: 24 GFLOPS
- Achieved: ~18 GFLOPS (75% efficiency, room for improvement)

**NAS EP (AI > 100):**
- Deeply compute-bound
- Expected: 24 GFLOPS
- Achieved: ~22 GFLOPS (92% efficiency, excellent)

## Conclusion

The Roofline model provides a powerful framework for understanding performance limitations and guiding optimisation efforts. By visualising the relationship between arithmetic intensity, memory bandwidth, and computational throughput, it enables systematic performance analysis and optimisation.

For RISC-V systems, constructing and analysing rooflines is particularly valuable given the diversity of implementations and the evolving state of compiler technology. The model helps identify whether performance gaps are due to hardware limitations, algorithm design, or implementation quality.

## References

- Williams, S., Waterman, A., & Patterson, D. (2009). Roofline: An Insightful Visual Performance Model for Multicore Architectures. Communications of the ACM, 52(4), 65-76.
- Ofenbeck, G., et al. (2014). Applying the Roofline Model. IEEE International Symposium on Performance Analysis of Systems and Software (ISPASS).
- Yang, C., et al. (2018). Roofline Model Toolkit: A Practical Tool for Architectural and Program Analysis. International Workshop on Performance Modeling, Benchmarking and Simulation of High Performance Computer Systems (PMBS).
