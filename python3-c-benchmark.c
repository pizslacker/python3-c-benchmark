#define PY_SSIZE_T_CLEAN
#include <Python.h>  // Must be included before standard headers
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define ARRAY_SIZE 100000000 // 100 million elements

// Atomic flag to control the spinner loop
volatile int keep_spinning = 1;

// ---------------------------------------------------------
// Spinner Thread Function
// ---------------------------------------------------------
void *spinner_thread(void *arg) {
    const char spinner[] = "|/-\\";
    int i = 0;
    
    while (keep_spinning) {
        printf("%c\b", spinner[i++ % 4]);
        fflush(stdout);
        usleep(100000); // 100ms delay
    }
    
    printf(" \b");
    return NULL;
}

// ---------------------------------------------------------
// 1. Standard C Implementation
// ---------------------------------------------------------
long long sum_c(const long long *arr, size_t size) {
    long long sum = 0;
    for (size_t i = 0; i < size; i++) {
        sum += arr[i];
    }
    return sum;
}

// ---------------------------------------------------------
// 2. Inline Python 3 Implementation
// ---------------------------------------------------------
// This Python script uses ctypes to cast the raw C memory address 
// into a Python array object without duplicating the memory.
const char* py_code = 
    "import ctypes\n"
    "import numpy as np\n"
    "def sum_py(ptr, size):\n"
    "    # Cast the pointer to a NumPy array (still zero-copy)\n"
    "    arr = np.ctypeslib.as_array(ctypes.cast(ptr, ctypes.POINTER(ctypes.c_longlong)), shape=(size,))\n"
    "    # Let NumPy's underlying C code handle the sum\n"
    "    return sum(arr)\n";

// ---------------------------------------------------------
// Helper function for timings
// ---------------------------------------------------------
double get_time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main() {
    printf("Allocating array of %d elements...\n", ARRAY_SIZE);
    long long *arr = malloc(ARRAY_SIZE * sizeof(long long));
    if (!arr) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        arr[i] = 1;
    }

    // --- Initialize Python Environment ---
    Py_Initialize();
    
    // Run the inline script to load the function into Python's memory
    PyRun_SimpleString(py_code);
    
    // Retrieve the 'sum_py' function from the __main__ module
    PyObject *main_module = PyImport_AddModule("__main__");
    PyObject *global_dict = PyModule_GetDict(main_module);
    PyObject *sum_func = PyDict_GetItemString(global_dict, "sum_py");

    struct timespec start, end;
    long long result_c = 0, result_py = 0;
    double time_c, time_py;

    printf("Running benchmarks... ");
    fflush(stdout);

    // Start the spinner in a background thread
    pthread_t spinner;
    pthread_create(&spinner, NULL, spinner_thread, NULL);

    // --- Benchmark C Version ---
    clock_gettime(CLOCK_MONOTONIC, &start);
    result_c = sum_c(arr, ARRAY_SIZE);
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_c = get_time_diff(start, end);

    // --- Benchmark Python Version ---
    // Prepare arguments for Python: (ptr_address, size)
    PyObject *pArgs = PyTuple_New(2);
    PyTuple_SetItem(pArgs, 0, PyLong_FromVoidPtr((void*)arr)); // PyTuple_SetItem steals the reference
    PyTuple_SetItem(pArgs, 1, PyLong_FromSize_t(ARRAY_SIZE));

    clock_gettime(CLOCK_MONOTONIC, &start);
    // Call the Python function
    PyObject *pResult = PyObject_CallObject(sum_func, pArgs);
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    if (pResult != NULL) {
        result_py = PyLong_AsLongLong(pResult);
        Py_DECREF(pResult);
    }
    Py_DECREF(pArgs);
    time_py = get_time_diff(start, end);

    // Stop the spinner thread
    keep_spinning = 0;
    pthread_join(spinner, NULL);
    printf("\n\n");

    // --- Cleanup Environment ---
    Py_Finalize();

    // --- Output Results ---
    printf("[C Version]      Result: %lld | Time: %.6f seconds\n", result_c, time_c);
    printf("[Python3 Version] Result: %lld | Time: %.6f seconds\n", result_py, time_py);

    printf("\nDifference: C was %.2fx %s than Python3.\n", 
           (time_py > time_c) ? (time_py / time_c) : (time_c / time_py),
           (time_py > time_c) ? "faster" : "slower");

    free(arr);
    return 0;
}
