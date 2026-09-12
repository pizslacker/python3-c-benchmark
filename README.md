# python3-c-benchmark

C benchmark program showcasing compiled system implementation language performance, compared to scripted Python3 code.
It allocates a 1-billion element array, runs a single-threaded benchmark comparing a standard C loop to inline x86_64 assembly, and shows a spinner animation.

Because GCC is exceptionally good at optimizing C code, the flags you use to compile this will heavily impact the results.

This C program uses GCC inline Python3 with `ctypes` allows the inline Python code to read the raw C memory pointer directly. It compares a standard C implementation of an array-summation function against a handwritten Python3 version, using `POSIX` high-resolution timers (`clock_gettime`) to measure the performance of both.

`Pthread` is utilized to animate a spinning pipe indicator in one thread while running the benchmark functions in the main thread.

#### Example output:
```bash
Allocating array of 100000000 elements...
Running benchmarks...  

[C Version]      Result: 100000000 | Time: 0.034664 seconds
[Python3 Version] Result: 100000000 | Time: 7.252038 seconds

Difference: C was 209.21x faster than Python.

```
