CC = gcc
CFLAGS = -O3 -march=native
LDFLAGS = -lm

# Uncomment the following line to enable OpenMP parallelism
# CFLAGS += -fopenmp

# Array size (default: 10 million elements)
# Adjust to exceed last-level cache size
ARRAY_SIZE = 10000000

# Number of iterations
NTIMES = 10

TARGET = stream

all: $(TARGET)

$(TARGET): stream.c
	$(CC) $(CFLAGS) -DSTREAM_ARRAY_SIZE=$(ARRAY_SIZE) -DNTIMES=$(NTIMES) -o $(TARGET) stream.c $(LDFLAGS)

openmp: CFLAGS += -fopenmp
openmp: $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all openmp clean
