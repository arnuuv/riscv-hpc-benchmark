/*
 * OpenMP Vector Addition Example
 * 
 * This program demonstrates parallel vector addition using OpenMP.
 * It measures execution time and calculates throughput.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define VECTOR_SIZE 100000000  // 100 million elements
#define ITERATIONS 10

// Function to get wall-clock time in seconds
double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

// Serial vector addition for verification
void vector_add_serial(double *a, double *b, double *c, size_t n) {
    for (size_t i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

// Parallel vector addition using OpenMP
void vector_add_parallel(double *a, double *b, double *c, size_t n) {
    #pragma omp parallel for
    for (size_t i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

// Verify results
int verify_results(double *c1, double *c2, size_t n, double tolerance) {
    for (size_t i = 0; i < n; i++) {
        if (fabs(c1[i] - c2[i]) > tolerance) {
            printf("Verification failed at index %zu: %f != %f\n", i, c1[i], c2[i]);
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {
    double *a, *b, *c_serial, *c_parallel;
    double start_time, end_time;
    double serial_time, parallel_time;
    double min_serial_time = 1e9, min_parallel_time = 1e9;
    size_t n = VECTOR_SIZE;
    
    printf("========================================\n");
    printf("OpenMP Vector Addition Benchmark\n");
    printf("========================================\n\n");
    
    // Display OpenMP configuration
    #pragma omp parallel
    {
        #pragma omp master
        {
            printf("Number of threads: %d\n", omp_get_num_threads());
        }
    }
    
    printf("Vector size: %zu elements\n", n);
    printf("Memory per vector: %.2f MB\n", (n * sizeof(double)) / (1024.0 * 1024.0));
    printf("Total memory: %.2f MB\n", (3 * n * sizeof(double)) / (1024.0 * 1024.0));
    printf("Iterations: %d\n\n", ITERATIONS);
    
    // Allocate memory
    a = (double *)malloc(n * sizeof(double));
    b = (double *)malloc(n * sizeof(double));
    c_serial = (double *)malloc(n * sizeof(double));
    c_parallel = (double *)malloc(n * sizeof(double));
    
    if (!a || !b || !c_serial || !c_parallel) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    // Initialize vectors
    printf("Initializing vectors...\n");
    #pragma omp parallel for
    for (size_t i = 0; i < n; i++) {
        a[i] = (double)i;
        b[i] = (double)(n - i);
    }
    
    // Warm-up run
    printf("Performing warm-up run...\n");
    vector_add_parallel(a, b, c_parallel, n);
    
    // Serial execution
    printf("\nRunning serial version...\n");
    for (int iter = 0; iter < ITERATIONS; iter++) {
        start_time = get_time();
        vector_add_serial(a, b, c_serial, n);
        end_time = get_time();
        serial_time = end_time - start_time;
        
        if (serial_time < min_serial_time) {
            min_serial_time = serial_time;
        }
        
        printf("  Iteration %2d: %.6f seconds\n", iter + 1, serial_time);
    }
    
    // Parallel execution
    printf("\nRunning parallel version...\n");
    for (int iter = 0; iter < ITERATIONS; iter++) {
        start_time = get_time();
        vector_add_parallel(a, b, c_parallel, n);
        end_time = get_time();
        parallel_time = end_time - start_time;
        
        if (parallel_time < min_parallel_time) {
            min_parallel_time = parallel_time;
        }
        
        printf("  Iteration %2d: %.6f seconds\n", iter + 1, parallel_time);
    }
    
    // Verify correctness
    printf("\nVerifying results...\n");
    if (verify_results(c_serial, c_parallel, n, 1e-9)) {
        printf("Verification: PASSED\n");
    } else {
        printf("Verification: FAILED\n");
        free(a); free(b); free(c_serial); free(c_parallel);
        return 1;
    }
    
    // Calculate and display performance metrics
    printf("\n========================================\n");
    printf("Performance Results\n");
    printf("========================================\n\n");
    
    printf("Best serial time:   %.6f seconds\n", min_serial_time);
    printf("Best parallel time: %.6f seconds\n", min_parallel_time);
    printf("Speedup:            %.2fx\n", min_serial_time / min_parallel_time);
    printf("Efficiency:         %.2f%%\n", 
           (min_serial_time / min_parallel_time) / omp_get_max_threads() * 100.0);
    
    // Calculate bandwidth (3 arrays accessed: 2 reads + 1 write)
    double bytes_transferred = 3.0 * n * sizeof(double);
    double serial_bandwidth = bytes_transferred / min_serial_time / (1024.0 * 1024.0 * 1024.0);
    double parallel_bandwidth = bytes_transferred / min_parallel_time / (1024.0 * 1024.0 * 1024.0);
    
    printf("\nMemory Bandwidth:\n");
    printf("  Serial:   %.2f GB/s\n", serial_bandwidth);
    printf("  Parallel: %.2f GB/s\n", parallel_bandwidth);
    
    printf("\n========================================\n");
    
    // Clean up
    free(a);
    free(b);
    free(c_serial);
    free(c_parallel);
    
    return 0;
}
