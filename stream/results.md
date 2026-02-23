# STREAM Benchmark Results

## System Configuration

**Note:** Document your system configuration here before recording results.

- **Processor:**
- **Core count:**
- **Cache sizes:** L1d, L1i, L2, L3
- **Memory:** Capacity, type, channels
- **Compiler:** Version and flags used
- **Operating System:**

## Compilation Parameters

- **Array size:** 10,000,000 elements (adjust based on cache size)
- **Element type:** double (8 bytes)
- **Total memory:** Approximately 229 MiB
- **Iterations:** 10
- **Compiler flags:** `-O3 -march=native`

## Results

### Serial Execution

```
Function    Best Rate MB/s  Avg time     Min time     Max time
Copy:       [FILL IN]       [FILL IN]    [FILL IN]    [FILL IN]
Scale:      [FILL IN]       [FILL IN]    [FILL IN]    [FILL IN]
Add:        [FILL IN]       [FILL IN]    [FILL IN]    [FILL IN]
Triad:      [FILL IN]       [FILL IN]    [FILL IN]    [FILL IN]
```

### OpenMP Parallel Execution

#### 1 Thread

```
Function    Best Rate MB/s  Avg time     Min time     Max time
Copy:       [FILL IN]       [FILL IN]    [FILL IN]    [FILL IN]
Scale:      [FILL IN]       [FILL IN]    [FILL IN]    [FILL IN]
Add:        [FILL IN]       [FILL IN]    [FILL IN]    [FILL IN]
Triad:      [FILL IN]       [FILL IN]    [FILL IN]    [FILL IN]
```

#### 2 Threads

```
Function    Best Rate MB/s  Avg time     Min time     Max time
Copy:       [FILL IN]       [FILL IN]    [FILL IN]    [FILL IN]
Scale:      [FILL IN]       [FILL IN]    [FILL IN]    [FILL IN]
Add:        [FILL IN]       [FILL IN]    [FILL IN]    [FILL IN]
Triad:      [FILL IN]       [FILL IN]    [FILL IN]    [FILL IN]
```

#### 4 Threads

```
Function    Best Rate MB/s  Avg time     Min time     Max time
Copy:       [FILL IN]       [FILL IN]    [FILL IN]    [FILL IN]
Scale:      [FILL IN]       [FILL IN]    [FILL IN]    [FILL IN]
Add:        [FILL IN]       [FILL IN]    [FILL IN]    [FILL IN]
Triad:      [FILL IN]       [FILL IN]    [FILL IN]    [FILL IN]
```

## Interpretation

### Memory Bandwidth Analysis

The STREAM Triad operation (`a[i] = b[i] + scalar * c[i]`) is typically the most representative of real application behaviour. It performs:
- 2 loads (b[i] and c[i])
- 1 store (a[i])
- 2 floating-point operations (multiply and add)

**Theoretical Peak Bandwidth:**

Calculate based on your system specifications:
- Memory frequency × memory bus width × number of channels

**Observed Bandwidth:**

Document the Triad bandwidth observed in your experiments.

**Efficiency:**

Efficiency = (Observed Bandwidth / Theoretical Peak Bandwidth) × 100%

Typical efficiency ranges from 70% to 90% on well-designed systems.

### Scaling Behaviour

Document how bandwidth scales with thread count:

- Does bandwidth increase linearly with threads?
- At what thread count does scaling saturate?
- Is there evidence of memory contention or NUMA effects?

### Kernel Comparison

The four STREAM kernels have different characteristics:

- **Copy:** Simple memory-to-memory transfer
- **Scale:** Adds one floating-point operation per element
- **Add:** Three array accesses
- **Triad:** Three array accesses plus two floating-point operations

Compare the bandwidth achieved by each kernel. In memory-bound scenarios, all four kernels should achieve similar bandwidth since the memory subsystem is the bottleneck.

## Observations

**Note:** Document specific observations from your experiments here.

Consider:
- Comparison with theoretical peak bandwidth
- Impact of compiler optimisations
- Effect of array size on performance
- Thread scaling efficiency
- Comparison with other architectures (if available)

## Recommendations for Future Experiments

- Vary array size to study cache effects
- Test with different compiler flags (see `tools/compiler_flags.md`)
- Investigate NUMA effects on multi-socket systems
- Compare with STREAM results from other RISC-V implementations
- Measure bandwidth under different system load conditions
