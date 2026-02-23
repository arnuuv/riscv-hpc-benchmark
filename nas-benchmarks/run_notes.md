# NAS Parallel Benchmarks: Compilation and Execution Guide

## Obtaining the Benchmarks

The NAS Parallel Benchmarks can be obtained from:
- Official NASA repository: https://www.nas.nasa.gov/software/npb.html
- OpenMP implementation: NPB3.4 or later

Download and extract the appropriate version for your system.

## Directory Structure

After extraction, the NPB directory structure typically looks like:

```
NPB3.4-OMP/
├── README
├── config/
│   ├── make.def.template
│   └── NAS.samples/
├── bin/
├── common/
└── [benchmark directories: EP, CG, FT, MG, IS, BT, SP, LU]
```

## Configuration

### Creating make.def

The primary configuration file is `config/make.def`. This file must be created before compilation.

1. Copy the template or a sample configuration:

```bash
cd NPB3.4-OMP/config
cp make.def.template make.def
```

2. Edit `make.def` for your RISC-V system (see `config_examples.md` for specific examples).

### Key Configuration Parameters

**Compiler Selection:**
```make
FC = gfortran
CC = gcc
```

**Optimisation Flags:**
```make
FFLAGS = -O3 -fopenmp -march=native
CFLAGS = -O3 -fopenmp -march=native
```

**Linking:**
```make
FLINKFLAGS = -O3 -fopenmp
CLINKFLAGS = -O3 -fopenmp
```

## Compilation

### Building Individual Benchmarks

To build a specific benchmark with a specific class:

```bash
make <benchmark> CLASS=<class>
```

Examples:
```bash
make ep CLASS=A        # Build EP benchmark, Class A
make cg CLASS=B        # Build CG benchmark, Class B
make ft CLASS=W        # Build FT benchmark, Class W
```

### Building All Benchmarks

To build all benchmarks for a specific class:

```bash
make suite CLASS=A
```

### Building Multiple Classes

To build multiple classes:

```bash
make ep CLASS=A CLASS=B CLASS=C
```

### Cleaning

To remove compiled binaries:

```bash
make clean
```

To remove binaries and configuration:

```bash
make veryclean
```

## Execution

### Running Benchmarks

Compiled binaries are placed in the `bin/` directory with names like `<benchmark>.<class>.x`.

Execute directly:

```bash
cd bin
./ep.A.x
```

### Setting Thread Count

For OpenMP versions, control thread count with `OMP_NUM_THREADS`:

```bash
export OMP_NUM_THREADS=4
./ep.A.x
```

### Thread Affinity

For consistent performance, consider setting thread affinity:

```bash
export OMP_PROC_BIND=close
export OMP_PLACES=cores
export OMP_NUM_THREADS=4
./ep.A.x
```

### Running Multiple Configurations

Example script to test thread scaling:

```bash
#!/bin/bash

BENCHMARK="ep.A.x"

for threads in 1 2 4 8 16; do
    echo "Running with $threads threads"
    export OMP_NUM_THREADS=$threads
    ./bin/$BENCHMARK
    echo ""
done
```

## Interpreting Output

### Standard Output Format

NPB benchmarks produce standardised output:

```
 NAS Parallel Benchmarks (NPB3.4-OMP) - EP Benchmark

 Number of random numbers generated:     536870912
 Number of active processes:                      4

EP Benchmark Results:

 CPU Time =    5.2341
 N = 2^   28
 No. Gaussian Pairs =      210832767.
 Sums =     -3.247834652034740D+03   -6.958407078382297D+03
 Counts:
  0      393058470.
  1      375280898.
  2       70460742.
  3        6291444.
  4         396844.
  5          12584.
  6              8.
  7              0.
  8              0.
  9              0.


 EP Benchmark Completed.
 Class           =                        A
 Size            =                536870912
 Iterations      =                        0
 Time in seconds =                     5.23
 Total threads   =                        4
 Avail threads   =                        4
 Mop/s total     =                  1027.45
 Operation type  = Random numbers generated
 Verification    =               SUCCESSFUL
```

### Key Metrics

- **Time in seconds:** Wall-clock execution time
- **Mop/s total:** Million operations per second (aggregate across all threads)
- **Verification:** Confirms correctness of computation

### Verification Status

- **SUCCESSFUL:** Computation completed correctly
- **UNSUCCESSFUL:** Numerical error detected (investigate compiler flags or hardware issues)
- **NOT PERFORMED:** Verification skipped (some configurations)

## Problem Class Selection

### Guidelines for RISC-V Systems

**Class S (Small):**
- Quick functionality testing
- Suitable for embedded RISC-V systems
- Execution time: seconds

**Class W (Workstation):**
- Single-core or low-core-count systems
- Initial performance characterisation
- Execution time: seconds to minutes

**Class A:**
- Standard small test
- Suitable for most RISC-V development boards
- Execution time: minutes

**Class B:**
- Standard medium test
- Requires substantial memory (several GB)
- Execution time: tens of minutes

**Class C and above:**
- Large-scale systems only
- Significant memory requirements (tens to hundreds of GB)
- Execution time: hours

### Memory Requirements

Approximate memory requirements vary by benchmark, but as a rough guide:

| Class | Typical Memory |
|-------|----------------|
| S     | < 100 MB       |
| W     | < 500 MB       |
| A     | 1-4 GB         |
| B     | 4-16 GB        |
| C     | 16-64 GB       |
| D     | 64-256 GB      |

Check specific benchmark documentation for precise requirements.

## Troubleshooting

### Compilation Errors

**Fortran compiler not found:**
- Install gfortran: `sudo apt-get install gfortran` (Debian/Ubuntu)
- Or use another Fortran compiler (ifort, flang)

**Undefined reference to OpenMP functions:**
- Ensure `-fopenmp` is in both `FFLAGS`/`CFLAGS` and `FLINKFLAGS`/`CLINKFLAGS`

**Architecture-specific errors:**
- Try removing `-march=native` or use `-march=rv64gc` explicitly
- Check compiler version compatibility with RISC-V target

### Runtime Errors

**Segmentation fault:**
- Problem class may be too large for available memory
- Try smaller class
- Check stack size: `ulimit -s unlimited`

**Incorrect results (verification fails):**
- Compiler optimisation issue: try `-O2` instead of `-O3`
- Floating-point precision: check compiler flags
- Hardware issue: run diagnostics

**Poor performance:**
- Verify thread count: `echo $OMP_NUM_THREADS`
- Check CPU frequency scaling
- Ensure system is not under load from other processes
- Review compiler optimisation flags

## Performance Data Collection

### Recommended Practice

1. **Warm-up run:** Execute once to load libraries and stabilise system
2. **Multiple runs:** Execute 3-5 times and report median or best time
3. **Document configuration:** Record all relevant system and compiler information
4. **Thread scaling study:** Test with 1, 2, 4, ... threads up to core count
5. **Compare with baseline:** If possible, compare with x86-64 or ARM results

### Automation Script Example

```bash
#!/bin/bash

BENCHMARK="ep.A.x"
RUNS=5

for threads in 1 2 4 8; do
    echo "Configuration: $threads threads"
    export OMP_NUM_THREADS=$threads
    
    for run in $(seq 1 $RUNS); do
        echo "  Run $run:"
        ./bin/$BENCHMARK | grep "Time in seconds"
    done
    echo ""
done
```

## References

- NPB Documentation: https://www.nas.nasa.gov/software/npb.html
- OpenMP Specification: https://www.openmp.org/specifications/
