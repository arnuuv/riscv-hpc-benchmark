# NAS Parallel Benchmarks: Configuration Examples

## Overview

This document provides example `make.def` configurations for compiling the NAS Parallel Benchmarks on RISC-V systems. The `make.def` file must be placed in the `config/` directory of the NPB distribution.

## Basic RISC-V Configuration (GCC)

### Conservative Optimisation

Suitable for initial testing and ensuring correctness:

```make
#---------------------------------------------------------------------------
# RISC-V System with GCC - Conservative Settings
#---------------------------------------------------------------------------

# Fortran compiler
FC = gfortran
FFLAGS = -O2 -fopenmp

# C compiler
CC = gcc
CFLAGS = -O2 -fopenmp

# Linker
FLINK = $(FC)
FLINKFLAGS = -O2 -fopenmp

CLINK = $(CC)
CLINKFLAGS = -O2 -fopenmp

# Random number generator
RAND = randi8

# Enable timing
WTIME = wtime.c
```

### Aggressive Optimisation

For maximum performance after verifying correctness:

```make
#---------------------------------------------------------------------------
# RISC-V System with GCC - Aggressive Optimisation
#---------------------------------------------------------------------------

# Fortran compiler
FC = gfortran
FFLAGS = -O3 -fopenmp -march=rv64gc -mtune=generic-rv64 -funroll-loops

# C compiler
CC = gcc
CFLAGS = -O3 -fopenmp -march=rv64gc -mtune=generic-rv64 -funroll-loops

# Linker
FLINK = $(FC)
FLINKFLAGS = -O3 -fopenmp

CLINK = $(CC)
CLINKFLAGS = -O3 -fopenmp

# Random number generator
RAND = randi8

# Enable timing
WTIME = wtime.c
```

### Native Architecture Optimisation

Optimise for the specific RISC-V system being used:

```make
#---------------------------------------------------------------------------
# RISC-V System with GCC - Native Architecture
#---------------------------------------------------------------------------

# Fortran compiler
FC = gfortran
FFLAGS = -O3 -fopenmp -march=native -mtune=native -funroll-loops -ffast-math

# C compiler
CC = gcc
CFLAGS = -O3 -fopenmp -march=native -mtune=native -funroll-loops -ffast-math

# Linker
FLINK = $(FC)
FLINKFLAGS = -O3 -fopenmp

CLINK = $(CC)
CLINKFLAGS = -O3 -fopenmp

# Random number generator
RAND = randi8

# Enable timing
WTIME = wtime.c
```

**Note:** `-ffast-math` may affect numerical accuracy. Use with caution and verify results.

## RISC-V with Vector Extension (RVV)

If your RISC-V system supports the vector extension:

```make
#---------------------------------------------------------------------------
# RISC-V System with Vector Extension
#---------------------------------------------------------------------------

# Fortran compiler
FC = gfortran
FFLAGS = -O3 -fopenmp -march=rv64gcv -mtune=generic-rv64 -funroll-loops

# C compiler
CC = gcc
CFLAGS = -O3 -fopenmp -march=rv64gcv -mtune=generic-rv64 -funroll-loops

# Linker
FLINK = $(FC)
FLINKFLAGS = -O3 -fopenmp

CLINK = $(CC)
CLINKFLAGS = -O3 -fopenmp

# Random number generator
RAND = randi8

# Enable timing
WTIME = wtime.c
```

**Note:** Vector extension support requires recent compiler versions and appropriate hardware.

## LLVM/Clang Configuration

Alternative configuration using LLVM/Clang compilers:

```make
#---------------------------------------------------------------------------
# RISC-V System with LLVM/Clang
#---------------------------------------------------------------------------

# Fortran compiler (using flang)
FC = flang
FFLAGS = -O3 -fopenmp -march=rv64gc

# C compiler
CC = clang
CFLAGS = -O3 -fopenmp -march=rv64gc

# Linker
FLINK = $(FC)
FLINKFLAGS = -O3 -fopenmp

CLINK = $(CC)
CLINKFLAGS = -O3 -fopenmp

# Random number generator
RAND = randi8

# Enable timing
WTIME = wtime.c
```

**Note:** Flang support for RISC-V may be limited depending on version.

## Debugging Configuration

For debugging and development:

```make
#---------------------------------------------------------------------------
# RISC-V System - Debug Configuration
#---------------------------------------------------------------------------

# Fortran compiler
FC = gfortran
FFLAGS = -g -O0 -fopenmp -Wall -Wextra -fcheck=all

# C compiler
CC = gcc
CFLAGS = -g -O0 -fopenmp -Wall -Wextra

# Linker
FLINK = $(FC)
FLINKFLAGS = -g -fopenmp

CLINK = $(CC)
CLINKFLAGS = -g -fopenmp

# Random number generator
RAND = randi8

# Enable timing
WTIME = wtime.c
```

## Compiler Flag Explanations

### Optimisation Levels

- **-O0:** No optimisation (debugging)
- **-O1:** Basic optimisation
- **-O2:** Moderate optimisation (recommended for correctness testing)
- **-O3:** Aggressive optimisation (recommended for performance)

### Architecture Flags

- **-march=rv64gc:** Target RISC-V 64-bit with general and compressed instructions
- **-march=rv64gcv:** Additionally enable vector extension
- **-march=native:** Optimise for the build machine's architecture
- **-mtune=generic-rv64:** Tune for generic RISC-V 64-bit processors
- **-mtune=native:** Tune for the build machine's specific processor

### OpenMP

- **-fopenmp:** Enable OpenMP directives and link OpenMP runtime library

### Additional Optimisations

- **-funroll-loops:** Unroll loops for better performance
- **-ffast-math:** Enable aggressive floating-point optimisations (may affect accuracy)
- **-finline-functions:** Inline functions aggressively
- **-ftree-vectorize:** Enable auto-vectorisation (enabled by default at -O3)

### Debugging and Diagnostics

- **-g:** Include debugging information
- **-Wall:** Enable common warnings
- **-Wextra:** Enable additional warnings
- **-fcheck=all:** Enable runtime checks (Fortran)

## Random Number Generator Selection

NPB supports different random number generators:

```make
# Standard portable generator
RAND = randi8

# Alternative (if available)
RAND = randdp
```

The `randi8` generator is recommended for portability.

## Timing Implementation

NPB requires a timing function. The standard implementation uses `wtime.c`:

```make
WTIME = wtime.c
```

This uses `gettimeofday()` for wall-clock timing.

## Platform-Specific Considerations

### SiFive Boards

For SiFive RISC-V boards (e.g., HiFive Unmatched):

```make
FC = gfortran
FFLAGS = -O3 -fopenmp -march=rv64gc -mtune=sifive-u74

CC = gcc
CFLAGS = -O3 -fopenmp -march=rv64gc -mtune=sifive-u74
```

### QEMU Emulation

When running under QEMU emulation:

```make
FC = gfortran
FFLAGS = -O2 -fopenmp -march=rv64gc -static

CC = gcc
CFLAGS = -O2 -fopenmp -march=rv64gc -static
```

Static linking may improve compatibility in emulated environments.

## Verification

After compilation, always verify correctness before performance testing:

1. Compile with conservative flags (-O2)
2. Run Class S or W
3. Verify "SUCCESSFUL" status
4. Then proceed to aggressive optimisation

If verification fails with aggressive optimisation:

1. Remove `-ffast-math`
2. Reduce optimisation to `-O2`
3. Remove `-funroll-loops`
4. Test incrementally to identify problematic flag

## Performance Tuning Strategy

1. **Baseline:** Start with `-O2 -fopenmp`
2. **Architecture:** Add `-march=rv64gc`
3. **Tuning:** Add `-mtune=native` or specific target
4. **Aggressive:** Increase to `-O3`
5. **Loop optimisation:** Add `-funroll-loops`
6. **Vectorisation:** Verify auto-vectorisation with `-fopt-info-vec`
7. **Fast math:** Cautiously add `-ffast-math` if accuracy permits

Test and verify at each step.

## Example Complete make.def

```make
#---------------------------------------------------------------------------
# Complete make.def for RISC-V NPB3.4-OMP
#---------------------------------------------------------------------------

SHELL = /bin/sh

# Fortran compiler and flags
FC = gfortran
FFLAGS = -O3 -fopenmp -march=rv64gc -mtune=native -funroll-loops

# C compiler and flags
CC = gcc
CFLAGS = -O3 -fopenmp -march=rv64gc -mtune=native -funroll-loops

# Fortran linker and flags
FLINK = $(FC)
FLINKFLAGS = -O3 -fopenmp

# C linker and flags
CLINK = $(CC)
CLINKFLAGS = -O3 -fopenmp

# Fortran libraries
FLIBS =

# C libraries
CLIBS = -lm

# Random number generator
RAND = randi8

# Timing routine
WTIME = wtime.c

# Enable additional optimisations (optional)
# UMR = 

#---------------------------------------------------------------------------
# End of make.def
#---------------------------------------------------------------------------
```

Save this as `config/make.def` in your NPB directory, adjust flags as needed, and proceed with compilation as described in `run_notes.md`.
