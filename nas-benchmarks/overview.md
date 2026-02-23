# NAS Parallel Benchmarks Overview

## Introduction

The NAS Parallel Benchmarks (NPB) are a suite of programs designed to evaluate the performance of parallel supercomputers. Originally developed at NASA Ames Research Center, these benchmarks are derived from computational fluid dynamics (CFD) applications and are widely used in the high-performance computing community to assess system performance and scalability.

The benchmarks consist of five kernels and three pseudo-applications, each designed to test different aspects of parallel computing performance.

## Problem Classes

NPB defines several problem classes (sizes) to accommodate systems of varying capability:

- **Class S (Small):** For quick testing and development
- **Class W (Workstation):** Small problem size for single-processor systems
- **Class A:** Standard test problem (small)
- **Class B:** Standard test problem (medium)
- **Class C:** Standard test problem (large)
- **Class D:** Large problem size
- **Class E:** Very large problem size
- **Class F:** Extremely large problem size

For RISC-V systems, it is recommended to start with Class S or W for initial testing, then progress to Class A and B as appropriate for the system capabilities.

## Kernel Benchmarks

### EP (Embarrassingly Parallel)

**Description:** Generates independent Gaussian random deviates using the Marsaglia polar method.

**Characteristics:**
- Minimal inter-processor communication
- Tests floating-point performance
- Highly scalable
- Representative of Monte Carlo applications

**Key Metrics:**
- Execution time
- Mop/s (million operations per second)

**Expected Behaviour:** Near-linear scaling with processor count due to minimal communication overhead.

### CG (Conjugate Gradient)

**Description:** Uses the conjugate gradient method to compute an approximation to the smallest eigenvalue of a large, sparse, symmetric positive definite matrix.

**Characteristics:**
- Irregular memory access patterns
- Tests unstructured grid computations
- Significant communication requirements
- Representative of iterative solvers

**Key Metrics:**
- Execution time
- Mop/s total

**Expected Behaviour:** Performance sensitive to memory bandwidth and latency. Communication patterns can stress interconnect on distributed systems.

### FT (Fourier Transform)

**Description:** Solves a three-dimensional partial differential equation using the Fast Fourier Transform (FFT).

**Characteristics:**
- All-to-all communication pattern
- Tests long-distance communication
- Regular memory access
- Representative of spectral methods

**Key Metrics:**
- Execution time
- Mop/s total

**Expected Behaviour:** Performance highly dependent on communication bandwidth and latency. Tests effectiveness of all-to-all communication primitives.

### MG (Multi-Grid)

**Description:** Uses a V-cycle multi-grid method to compute the solution of the three-dimensional scalar Poisson equation.

**Characteristics:**
- Tests both short and long-distance communication
- Structured grid computation
- Multiple grid levels with different communication patterns
- Representative of multi-grid solvers

**Key Metrics:**
- Execution time
- Mop/s total

**Expected Behaviour:** Performance depends on both computation and communication. Different grid levels stress different aspects of the memory hierarchy.

### IS (Integer Sort)

**Description:** Performs a large integer sort using a bucket sort algorithm.

**Characteristics:**
- Tests integer computation and communication
- Random memory access patterns
- All-to-all communication
- Representative of sorting and combinatorial applications

**Key Metrics:**
- Execution time
- Mop/s total

**Expected Behaviour:** Performance limited by memory bandwidth and communication latency. Stresses integer execution units rather than floating-point.

## Pseudo-Application Benchmarks

### BT (Block Tri-diagonal)

**Description:** Solves three sets of uncoupled systems of equations using block tri-diagonal matrices.

**Characteristics:**
- Structured grid computation
- Nearest-neighbour communication
- Representative of implicit CFD solvers
- Tests load balance

**Key Metrics:**
- Execution time
- Mop/s total

**Expected Behaviour:** Performance depends on efficient nearest-neighbour communication and computational throughput.

### SP (Scalar Penta-diagonal)

**Description:** Solves three sets of uncoupled systems of equations using scalar penta-diagonal matrices.

**Characteristics:**
- Similar to BT but with different matrix structure
- Nearest-neighbour communication
- Representative of implicit CFD solvers
- More fine-grained parallelism than BT

**Key Metrics:**
- Execution time
- Mop/s total

**Expected Behaviour:** Similar to BT, with performance depending on communication efficiency and computational throughput.

### LU (Lower-Upper Symmetric Gauss-Seidel)

**Description:** Solves a regular-sparse, block lower and upper triangular system using symmetric successive over-relaxation (SSOR).

**Characteristics:**
- Pipeline algorithm with wave-front propagation
- Tests ability to overlap computation and communication
- Representative of implicit CFD solvers
- Challenging load balance

**Key Metrics:**
- Execution time
- Mop/s total

**Expected Behaviour:** Performance sensitive to ability to overlap computation and communication. Pipeline nature can expose latency issues.

## Relevance to RISC-V Systems

The NAS Parallel Benchmarks are particularly valuable for RISC-V system evaluation because they:

1. **Test diverse computational patterns:** From regular (FT, MG) to irregular (CG, IS)
2. **Stress different system components:** Memory bandwidth (STREAM-like in some kernels), floating-point units (EP), integer units (IS)
3. **Evaluate parallel efficiency:** From embarrassingly parallel (EP) to communication-intensive (FT)
4. **Provide standardised comparison:** Results can be compared with established architectures
5. **Scale across problem sizes:** Classes S through F allow testing on systems from embedded to HPC

## Performance Analysis Considerations

When analysing NPB results on RISC-V systems, consider:

- **Memory bandwidth limitations:** Many kernels are memory-bound; compare with STREAM results
- **Floating-point performance:** Compare EP results with theoretical peak FLOPS
- **Communication overhead:** On shared-memory systems, evaluate OpenMP synchronisation costs
- **Compiler effectiveness:** RISC-V compiler maturity may impact optimisation quality
- **ISA extensions:** Vector extensions (RVV) may significantly impact performance if supported

## References

- Bailey, D. H., et al. (1991). The NAS Parallel Benchmarks. International Journal of High Performance Computing Applications, 5(3), 63-73.
- Jin, H., Frumkin, M., & Yan, J. (1999). The OpenMP Implementation of NAS Parallel Benchmarks and Its Performance. NASA Technical Report NAS-99-011.
