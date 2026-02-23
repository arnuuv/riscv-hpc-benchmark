# Compiler Optimisation Flags for RISC-V

## Introduction

Compiler optimisation flags significantly impact the performance of compiled code. Understanding these flags is essential for performance benchmarking, as different optimisation levels and architecture-specific options can produce dramatically different results.

This document explains commonly used compiler flags for GCC and LLVM/Clang when targeting RISC-V systems, with emphasis on flags relevant to high-performance computing applications.

## Optimisation Levels

### -O0 (No Optimisation)

**Purpose:** Debugging and development

**Characteristics:**
- No optimisation performed
- Direct translation of source code
- Fast compilation time
- Large binary size
- Slow execution
- Predictable debugging behaviour

**When to use:**
- Initial development
- Debugging with gdb
- Verifying correctness
- Understanding compiler-generated assembly

**Example:**
```bash
gcc -O0 -g -o program program.c
```

### -O1 (Basic Optimisation)

**Purpose:** Moderate optimisation without significant compilation time increase

**Characteristics:**
- Basic optimisations enabled
- Reasonable compilation time
- Moderate performance improvement
- Still debuggable

**Optimisations include:**
- Dead code elimination
- Basic register allocation
- Simple loop optimisations
- Common subexpression elimination

**When to use:**
- Early development when some performance is needed
- Systems with limited compilation resources
- Rarely used in benchmarking

### -O2 (Recommended Optimisation)

**Purpose:** Standard optimisation level for production code

**Characteristics:**
- Aggressive optimisation without space-time tradeoffs
- Longer compilation time than -O1
- Good performance
- Recommended default for most applications

**Additional optimisations beyond -O1:**
- Instruction scheduling
- Loop optimisations
- Function inlining (conservative)
- Vectorisation (limited)
- Strength reduction
- Register allocation improvements

**When to use:**
- Production builds
- Initial benchmarking
- When correctness is paramount
- Conservative optimisation approach

**Example:**
```bash
gcc -O2 -o program program.c
```

### -O3 (Aggressive Optimisation)

**Purpose:** Maximum performance optimisation

**Characteristics:**
- Most aggressive optimisation
- Longest compilation time
- May increase code size
- Best performance in most cases
- Recommended for benchmarking

**Additional optimisations beyond -O2:**
- Aggressive function inlining
- Loop unrolling
- Vectorisation (aggressive)
- Predictive commoning
- Loop interchange
- More aggressive instruction scheduling

**Potential issues:**
- May expose floating-point precision issues
- Increased code size
- Rare cases of incorrect optimisation (compiler bugs)

**When to use:**
- Performance benchmarking
- Production code after verification
- HPC applications
- When maximum performance is required

**Example:**
```bash
gcc -O3 -o program program.c
```

**Recommendation:** Always verify correctness at -O2 before using -O3.

### -Os (Optimise for Size)

**Purpose:** Minimise code size

**Characteristics:**
- Similar to -O2 but disables optimisations that increase code size
- Useful for embedded systems
- May reduce performance compared to -O2/-O3

**When to use:**
- Embedded RISC-V systems with limited memory
- Not recommended for performance benchmarking

### -Ofast (Aggressive with Fast Math)

**Purpose:** Maximum performance with relaxed standards compliance

**Characteristics:**
- Enables -O3 plus -ffast-math
- Violates strict IEEE 754 floating-point semantics
- May produce incorrect results for some programs
- Can provide significant speedup for floating-point code

**When to use:**
- Only when floating-point precision can be relaxed
- After careful verification
- Document clearly in benchmark results

**Example:**
```bash
gcc -Ofast -o program program.c
```

**Warning:** Use with caution. Always verify correctness.

## Architecture-Specific Flags

### -march (Target Architecture)

**Purpose:** Specify target RISC-V architecture and extensions

**Syntax:**
```bash
-march=<architecture>
```

**Common RISC-V architectures:**

**rv64g or rv64gc:**
- 64-bit RISC-V
- G = IMAFD (general-purpose extensions)
- C = Compressed instructions
- Standard baseline for RISC-V systems

```bash
gcc -march=rv64gc -o program program.c
```

**rv64gcv:**
- Adds V extension (vector operations)
- Requires hardware support
- Enables vectorisation

```bash
gcc -march=rv64gcv -o program program.c
```

**rv32gc:**
- 32-bit RISC-V
- For embedded systems

**native:**
- Auto-detect and use all features of the build machine
- Convenient but not portable
- Recommended for benchmarking on the target system

```bash
gcc -march=native -o program program.c
```

**Extension breakdown:**
- I: Integer base ISA
- M: Integer multiplication and division
- A: Atomic instructions
- F: Single-precision floating-point
- D: Double-precision floating-point
- C: Compressed instructions (16-bit)
- V: Vector extension

**When to use:**
- Always specify for performance-critical code
- Use -march=native for benchmarking on target system
- Use specific architecture for cross-compilation

### -mtune (Optimise for Specific Processor)

**Purpose:** Optimise code for a specific processor microarchitecture

**Syntax:**
```bash
-mtune=<processor>
```

**Common values:**

**generic-rv64:**
- Generic 64-bit RISC-V
- Default if not specified

**native:**
- Optimise for the build machine's processor

**sifive-u74:**
- SiFive U74 core (e.g., HiFive Unmatched)

**sifive-u54:**
- SiFive U54 core

**Difference from -march:**
- `-march`: Determines which instructions are available
- `-mtune`: Determines instruction scheduling and other microarchitecture-specific optimisations
- Code compiled with `-mtune` still runs on other processors, just potentially slower

**Example:**
```bash
gcc -march=rv64gc -mtune=sifive-u74 -o program program.c
```

**When to use:**
- When targeting a specific known processor
- For maximum performance on a specific system
- Use -mtune=native for benchmarking

## OpenMP Flags

### -fopenmp

**Purpose:** Enable OpenMP parallel programming

**Characteristics:**
- Enables OpenMP directives (#pragma omp)
- Links OpenMP runtime library
- Essential for parallel benchmarks

**Example:**
```bash
gcc -O3 -fopenmp -o program program.c
```

**Usage:**
```c
#pragma omp parallel for
for (int i = 0; i < n; i++) {
    // parallel work
}
```

**Thread control:**
```bash
export OMP_NUM_THREADS=4
./program
```

**When to use:**
- All OpenMP programs
- Parallel benchmarks (STREAM, NAS, custom microbenchmarks)
- Multi-threaded applications

**Note:** Must be specified for both compilation and linking.

## Vectorisation Flags

### Auto-Vectorisation

**-ftree-vectorize:**
- Enable auto-vectorisation
- Enabled by default at -O3
- Explicit specification at lower optimisation levels

```bash
gcc -O2 -ftree-vectorize -o program program.c
```

**-fopt-info-vec:**
- Report vectorisation information
- Useful for understanding what was vectorised

```bash
gcc -O3 -march=rv64gcv -fopt-info-vec -o program program.c
```

**-fopt-info-vec-missed:**
- Report missed vectorisation opportunities
- Helps identify why loops were not vectorised

```bash
gcc -O3 -march=rv64gcv -fopt-info-vec-missed -o program program.c
```

**-fopt-info-vec-all:**
- Report all vectorisation information

### Vectorisation Requirements

For successful auto-vectorisation:
1. Loop bounds must be known or easily determined
2. No loop-carried dependencies
3. Memory accesses should be contiguous or strided
4. Target architecture must support vector operations (-march=rv64gcv)

**Example:**
```c
// Vectorisable
for (int i = 0; i < n; i++) {
    c[i] = a[i] + b[i];
}

// Not vectorisable (loop-carried dependency)
for (int i = 1; i < n; i++) {
    a[i] = a[i-1] + b[i];
}
```

## Loop Optimisation Flags

### -funroll-loops

**Purpose:** Unroll loops to reduce overhead and increase instruction-level parallelism

**Characteristics:**
- Reduces loop control overhead
- Increases code size
- May improve performance for small loops
- Can enable further optimisations

**Example:**
```bash
gcc -O3 -funroll-loops -o program program.c
```

**When to use:**
- Compute-intensive loops
- After verifying benefit (may not always help)
- HPC applications

### -funroll-all-loops

**Purpose:** Unroll all loops, even those with unknown bounds

**Characteristics:**
- More aggressive than -funroll-loops
- Can significantly increase code size
- May hurt performance in some cases

**When to use:**
- Rarely; -funroll-loops is usually sufficient

### -floop-nest-optimize

**Purpose:** Enable loop nest optimisations (Graphite framework)

**Characteristics:**
- Loop interchange
- Loop tiling/blocking
- Loop fusion
- Advanced loop transformations

**Example:**
```bash
gcc -O3 -floop-nest-optimize -o program program.c
```

**When to use:**
- Complex nested loops
- Matrix operations
- Stencil computations

## Floating-Point Optimisation Flags

### -ffast-math

**Purpose:** Enable aggressive floating-point optimisations that violate IEEE 754

**Characteristics:**
- Assumes no NaNs or infinities
- Allows reassociation of operations
- Enables reciprocal approximations
- May produce incorrect results

**Implications:**
- (a + b) + c may be computed as a + (b + c)
- Division may be replaced with multiplication by reciprocal
- Math library functions may use approximations

**Example:**
```bash
gcc -O3 -ffast-math -o program program.c
```

**When to use:**
- Only when numerical accuracy can be relaxed
- After thorough verification
- Must be documented in benchmark results

**When NOT to use:**
- Financial calculations
- Scientific computing requiring strict accuracy
- When NaN or infinity handling is required

### -fno-math-errno

**Purpose:** Assume math functions do not set errno

**Characteristics:**
- Allows inlining of math functions
- Part of -ffast-math
- Generally safe

### -funsafe-math-optimizations

**Purpose:** Allow optimisations that may violate IEEE 754

**Characteristics:**
- Subset of -ffast-math
- Less aggressive than full -ffast-math

## Inlining Flags

### -finline-functions

**Purpose:** Inline functions even if not declared inline

**Characteristics:**
- Enabled at -O3
- Reduces function call overhead
- Increases code size
- Enables interprocedural optimisations

### -finline-limit=n

**Purpose:** Control the size of functions that can be inlined

**Characteristics:**
- Default varies by compiler version
- Larger values allow more inlining
- May increase compilation time and code size

**Example:**
```bash
gcc -O3 -finline-limit=1000 -o program program.c
```

## Link-Time Optimisation (LTO)

### -flto

**Purpose:** Enable link-time optimisation

**Characteristics:**
- Optimises across translation units
- Enables whole-program optimisation
- Longer compilation time
- Can provide significant performance improvements

**Example:**
```bash
gcc -O3 -flto -o program program1.c program2.c
```

**When to use:**
- Multi-file projects
- When maximum performance is required
- Production builds

**Note:** Must be specified for both compilation and linking.

## Debugging and Analysis Flags

### -g

**Purpose:** Include debugging information

**Characteristics:**
- Enables debugging with gdb
- Increases binary size
- Does not affect performance (usually)

**Example:**
```bash
gcc -O2 -g -o program program.c
```

**When to use:**
- Development
- Debugging optimised code
- Profiling with tools that need symbols

### -Wall, -Wextra

**Purpose:** Enable compiler warnings

**Characteristics:**
- Detect potential bugs
- Encourage good coding practices
- No performance impact

**Example:**
```bash
gcc -O3 -Wall -Wextra -o program program.c
```

**When to use:**
- Always, especially during development

## Recommended Flag Combinations

### Conservative Benchmarking

For initial benchmarking and correctness verification:

```bash
gcc -O2 -march=rv64gc -fopenmp -o program program.c -lm
```

### Aggressive Benchmarking

For maximum performance:

```bash
gcc -O3 -march=native -mtune=native -fopenmp -funroll-loops -o program program.c -lm
```

### With Vectorisation (if RVV available)

```bash
gcc -O3 -march=rv64gcv -mtune=native -fopenmp -funroll-loops -fopt-info-vec -o program program.c -lm
```

### With LTO (multi-file projects)

```bash
gcc -O3 -march=native -mtune=native -fopenmp -flto -o program file1.c file2.c -lm
```

### Maximum Performance (use with caution)

```bash
gcc -Ofast -march=native -mtune=native -fopenmp -funroll-loops -flto -o program program.c -lm
```

**Note:** Always verify correctness before using aggressive flags.

## LLVM/Clang Specific Flags

Most flags are similar between GCC and Clang, but some differences exist:

### Clang-Specific

**-Rpass=vec:**
- Report vectorisation (similar to -fopt-info-vec)

**-Rpass-missed=vec:**
- Report missed vectorisation

**-Rpass-analysis=vec:**
- Report vectorisation analysis

**Example:**
```bash
clang -O3 -march=rv64gcv -Rpass=vec -o program program.c
```

## Performance Tuning Workflow

### Step 1: Baseline

Compile with conservative flags:
```bash
gcc -O2 -march=rv64gc -o program program.c
```

### Step 2: Verify Correctness

Run tests to ensure correct behaviour.

### Step 3: Increase Optimisation

```bash
gcc -O3 -march=rv64gc -o program program.c
```

Verify correctness again.

### Step 4: Add Architecture-Specific Flags

```bash
gcc -O3 -march=native -mtune=native -o program program.c
```

### Step 5: Add Additional Optimisations

```bash
gcc -O3 -march=native -mtune=native -funroll-loops -o program program.c
```

### Step 6: Consider Aggressive Flags (if appropriate)

```bash
gcc -Ofast -march=native -mtune=native -funroll-loops -o program program.c
```

Verify correctness carefully.

### Step 7: Measure and Compare

- Measure performance at each step
- Document which flags provide benefit
- Keep flags that improve performance
- Remove flags that hurt performance or increase compilation time without benefit

## Common Pitfalls

### Using -march=native for Cross-Compilation

**Problem:** Binary will not run on target if compiled on different architecture

**Solution:** Specify exact target architecture

### Forgetting -fopenmp at Link Time

**Problem:** Undefined references to OpenMP functions

**Solution:** Include -fopenmp for both compilation and linking

### Using -ffast-math Without Verification

**Problem:** Incorrect numerical results

**Solution:** Verify correctness; document usage in results

### Over-Optimisation

**Problem:** Excessive compilation time, code size, or even reduced performance

**Solution:** Measure impact of each flag; use only beneficial flags

## Documentation Best Practices

When reporting benchmark results, always document:

- Compiler version: `gcc --version`
- Complete compilation command
- All optimisation flags used
- Target architecture
- Any non-standard flags

**Example:**
```
Compiler: GCC 12.2.0
Command: gcc -O3 -march=rv64gc -mtune=native -fopenmp -funroll-loops -o stream stream.c -lm
```

This ensures reproducibility and fair comparison.

## References

- GCC Documentation: https://gcc.gnu.org/onlinedocs/
- RISC-V GCC Documentation: https://github.com/riscv-collab/riscv-gnu-toolchain
- LLVM/Clang Documentation: https://clang.llvm.org/docs/
- Fog, A. (2004-2023). Optimizing Software in C++. Technical University of Denmark.
