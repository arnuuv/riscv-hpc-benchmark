CC = gcc
CFLAGS = -O3 -march=native -fopenmp -Wall
LDFLAGS = -lm

# Matrix size for matmul (can be overridden)
MATRIX_SIZE = 1024

TARGETS = vector_add matmul

all: $(TARGETS)

vector_add: vector_add.c
	$(CC) $(CFLAGS) -o vector_add vector_add.c $(LDFLAGS)

matmul: matmul.c
	$(CC) $(CFLAGS) -DMATRIX_SIZE=$(MATRIX_SIZE) -o matmul matmul.c $(LDFLAGS)

# Build with different matrix sizes
matmul-512:
	$(CC) $(CFLAGS) -DMATRIX_SIZE=512 -o matmul-512 matmul.c $(LDFLAGS)

matmul-1024:
	$(CC) $(CFLAGS) -DMATRIX_SIZE=1024 -o matmul-1024 matmul.c $(LDFLAGS)

matmul-2048:
	$(CC) $(CFLAGS) -DMATRIX_SIZE=2048 -o matmul-2048 matmul.c $(LDFLAGS)

# Test with different thread counts
test-vector: vector_add
	@echo "Testing vector addition with different thread counts..."
	@for threads in 1 2 4 8; do \
		echo ""; \
		echo "=== Testing with $$threads thread(s) ==="; \
		OMP_NUM_THREADS=$$threads ./vector_add; \
	done

test-matmul: matmul
	@echo "Testing matrix multiplication with different thread counts..."
	@for threads in 1 2 4 8; do \
		echo ""; \
		echo "=== Testing with $$threads thread(s) ==="; \
		OMP_NUM_THREADS=$$threads ./matmul; \
	done

test: test-vector test-matmul

clean:
	rm -f $(TARGETS) matmul-512 matmul-1024 matmul-2048

.PHONY: all test test-vector test-matmul clean matmul-512 matmul-1024 matmul-2048
