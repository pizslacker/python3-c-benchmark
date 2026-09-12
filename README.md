# python3-c-benchmark

C benchmark program showcasing (compiled binary) system implementation language performance, compared to scripted/interpreted Python3 code.
It allocates a 100-million element array, runs a single-threaded benchmark comparing a standard C loop to inline idiomatic (_Pythonic_) `Python3` code, and shows a spinner animation.

This was also a fun learning excersize for me, using another language within C with a language header (`#include <Python.h>`).

This C program uses GCC inline `Python3` with `ctypes` allowing the inline `Python3` code to read the raw C memory pointer directly. It compares a standard C implementation of an array-summation function against a handwritten idiomatic `Python3` version, using `POSIX` high-resolution timers (`clock_gettime`) to measure the performance of both.

`Pthread` is utilized to animate a spinning pipe indicator in one thread while running the benchmark functions in the main thread.

#### Example output:
```bash
Allocating array of 100000000 elements...
Running benchmarks...  

[C Version]      Result: 100000000 | Time: 0.034822 seconds
[Python3 Version] Result: 100000000 | Time: 3.700200 seconds

Difference: C was 106.26x faster than Python3.
```
