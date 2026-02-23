/*
 * OpenMP Matrix Multiplication Example
 * 
 * This program demonstrates parallel matrix multiplication using OpenMP.
 * It implements the standard triple-nested loop algorithm and measures
 * execution time and FLOPS.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>

#ifndef MATRIX_SIZE
#define MATRIX_SIZE 1024
#endif

#define ITERATIONS 5

// Function to get wall-clock time in seconds
double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

// Initialize matrix with values
void initialize_matrix(double *matrix, int n, int seed) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i * n + j] = (double)((i + j + seed) % 100) / 10.0;
        }
    }
}

// Serial matrix multiplication
void matmul_serial(double *A, double *B, double *C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = sum;
        }
    }
}

// Parallel matrix multiplication using OpenMP
void matmul_parallel(double *A, double *B, double *C, int n) {
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = sum;
        }
    }
}

// Parallel matrix multiplication with loop collapse
void matmul_parallel_collapse(double *A, double *B, double *C, int n) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = sum;
        }
    }
}

// Verify results (compare two matrices)
int verify_results(double *C1, double *C2, int n, double tolerance) {
    int errors = 0;
    for (int i = 0; i < n * n; i++) {
        if (fabs(C1[i] - C2[i]) > tolerance) {
            errors++;
            if (errors <= 5) {  // Print first 5 errors
                printf("  Error at index %d: %.6f != %.6f (diff: %.6e)\n", 
                       i, C1[i], C2[i], fabs(C1[i] - C2[i]));
            }
        }
    }
    return errors;
}

int main(int argc, char *argv[]) {
    int n = MATRIX_SIZE;
    double *A, *B, *C_serial, *C_parallel, *C_collapse;
    double start_time, end_time;
    double serial_time, parallel_time, collapse_time;
    double min_serial_time = 1e9, min_parallel_time = 1e9, min_collapse_time = 1e9;
    
    printf("========================================\n");
    printf("OpenMP Matrix Multiplication Benchmark\n");
    printf("========================================\n\n");
    
    // Display OpenMP configuration
    #pragma omp parallel
    {
        #pragma omp master
        {
            printf("Number of threads: %d\n", omp_get_num_threads());
        }
    }
    
    printf("Matrix size: %d x %d\n", n, n);
    printf("Memory per matrix: %.2f MB\n", (n * n * sizeof(double)) / (1024.0 * 1024.0));
    printf("Total memory: %.2f MB\n", (4 * n * n * sizeof(double)) / (1024.0 * 1024.0));
    printf("Iterations: %d\n", ITERATIONS);
    printf("Operations per multiplication: %ld (2*n^3)\n", 2L * n * n * n);
    printf("\n");
    
    // Allocate memory
    A = (double *)malloc(n * n * sizeof(double));
    B = (double *)malloc(n * n * sizeof(double));
    C_serial = (double *)malloc(n * n * sizeof(double));
    C_parallel = (double *)malloc(n * n * sizeof(double));
    C_collapse = (double *)malloc(n * n * sizeof(double));
    
    if (!A || !B || !C_serial || !C_parallel || !C_collapse) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    // Initialize matrices
    printf("Initializing matrices...\n");
    initialize_matrix(A, n, 1);
    initialize_matrix(B, n, 2);
    
    // Warm-up run
    printf("Performing warm-up run...\n");
    matmul_parallel(A, B, C_parallel, n);
    
    // Serial execution
    printf("\nRunning serial version...\n");
    for (int iter = 0; iter < ITERATIONS; iter++) {
        start_time = get_time();
        matmul_serial(A, B, C_serial, n);
        end_time = get_time();
        serial_time = end_time - start_time;
        
        if (serial_time < min_serial_time) {
            min_serial_time = serial_time;
        }
        
        printf("  Iteration %d: %.6f seconds (%.2f GFLOPS)\n", 
               iter + 1, serial_time, (2.0 * n * n * n) / serial_time / 1e9);
    }
    
    // Parallel execution
    printf("\nRunning parallel version...\n");
    for (int iter = 0; iter < ITERATIONS; iter++) {
        start_time = get_time();
        matmul_parallel(A, B, C_parallel, n);
        end_time = get_time();
        parallel_time = end_time - start_time;
        
        if (parallel_time < min_parallel_time) {
            min_parallel_time = parallel_time;
        }
        
        printf("  Iteration %d: %.6f seconds (%.2f GFLOPS)\n", 
               iter + 1, parallel_time, (2.0 * n * n * n) / parallel_time / 1e9);
    }
    
    // Parallel execution with collapse
    printf("\nRunning parallel version with collapse(2)...\n");
    for (int iter = 0; iter < ITERATIONS; iter++) {
        start_time = get_time();
        matmul_parallel_collapse(A, B, C_collapse, n);
        end_time = get_time();
        collapse_time = end_time - start_time;
        
        if (collapse_time < min_collapse_time) {
            min_collapse_time = collapse_time;
        }
        
        printf("  Iteration %d: %.6f seconds (%.2f GFLOPS)\n", 
               iter + 1, collapse_time, (2.0 * n * n * n) / collapse_time / 1e9);
    }
    
    // Verify correctness
    printf("\nVerifying results...\n");
    
    printf("Comparing parallel vs serial:\n");
    int errors_parallel = verify_results(C_serial, C_parallel, n, 1e-6);
    if (errors_parallel == 0) {
        printf("  Verification: PASSED\n");
    } else {
        printf("  Verification: FAILED (%d errors)\n", errors_parallel);
    }
    
    printf("Comparing collapse vs serial:\n");
    int errors_collapse = verify_results(C_serial, C_collapse, n, 1e-6);
    if (errors_collapse == 0) {
        printf("  Verification: PASSED\n");
    } else {
        printf("  Verification: FAILED (%d errors)\n", errors_collapse);
    }
    
    // Calculate and display performance metrics
    printf("\n========================================\n");
    printf("Performance Results\n");
    printf("========================================\n\n");
    
    double flops = 2.0 * n * n * n;
    
    printf("Best execution times:\n");
    printf("  Serial:             %.6f seconds\n", min_serial_time);
    printf("  Parallel:           %.6f seconds\n", min_parallel_time);
    printf("  Parallel (collapse): %.6f seconds\n", min_collapse_time);
    
    printf("\nPerformance (GFLOPS):\n");
    printf("  Serial:             %.2f GFLOPS\n", flops / min_serial_time / 1e9);
    printf("  Parallel:           %.2f GFLOPS\n", flops / min_parallel_time / 1e9);
    printf("  Parallel (collapse): %.2f GFLOPS\n", flops / min_collapse_time / 1e9);
    
    printf("\nSpeedup:\n");
    printf("  Parallel:           %.2fx\n", min_serial_time / min_parallel_time);
    printf("  Parallel (collapse): %.2fx\n", min_serial_time / min_collapse_time);
    
    int num_threads = omp_get_max_threads();
    printf("\nParallel Efficiency:\n");
    printf("  Parallel:           %.2f%%\n", 
           (min_serial_time / min_parallel_time) / num_threads * 100.0);
    printf("  Parallel (collapse): %.2f%%\n", 
           (min_serial_time / min_collapse_time) / num_threads * 100.0);
    
    printf("\n========================================\n");
    
    // Clean up
    free(A);
    free(B);
    free(C_serial);
    free(C_parallel);
    free(C_collapse);
    
    return 0;
}
