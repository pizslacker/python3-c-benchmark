# Variables
CC = gcc
CFLAGS = -O3 -Wall $(shell python3-config --cflags --embed)
LDFLAGS = $(shell python3-config --ldflags --embed) -lpthread
TARGET = python3-c-benchmark
SRC = python3-c-benchmark.c

# Default rule
all: $(TARGET)

# Compile the target
$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

# Clean up build artifacts
clean:
	rm -f $(TARGET)