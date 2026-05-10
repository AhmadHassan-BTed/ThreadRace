# System Architecture & Technical Design

This document provides a comprehensive technical overview of the **Typical vs Threaded Program Time Tester**, outlining the engineering principles and algorithmic choices that drive its benchmarking capabilities.

## 🧠 Core Engineering Logic

The fundamental objective of this utility is to provide an empirical comparison between synchronous and asynchronous execution models. To achieve this, the system requires a computational task that is both deterministic and highly demanding of system resources.

### The Choice of the Ackermann Function

Rather than utilizing a standard sleep delay or a simple arithmetic loop, the project integrates the **Ackermann function**. This choice was deliberate:
- **Recursive Depth**: The Ackermann function is famously not primitive recursive, meaning it forces the CPU to manage a massive stack of nested calls.
- **Resource Stress**: By executing `Ackermann(4, 1)`, the program generates millions of recursive transitions. This creates a realistic "heavy load" that challenges the kernel's thread scheduler and the processor's branch predictor.
- **Fair Comparison**: Because the mathematical work is identical in both modes, the only variable in the benchmark is the execution strategy (sequential vs. parallel).

## 🏗 Modular Components

### 1. The Compute Engine (`src/main.cpp`)
- **Execution Orchestrator**: The `main` function acts as a controller, switching logic based on the user-provided mode (1 or 2).
- **Concurrency Implementation**: Mode 2 utilizes `pthread_create` and `pthread_join`. This implementation is designed to show the overhead of thread lifecycle management versus the speed gains of multi-core parallelism.
- **Error Handling**: The system includes checks for thread creation failures, ensuring reliability during high-concurrency tests.

### 2. Build & Optimization (`Makefile`)
The build system is tuned to produce a production-grade binary:
- **Optimization**: Uses the `-O2` flag to ensure the compiler doesn't artificially slow down the sequential logic, while still allowing the threading logic to shine.
- **Linking**: Explicitly links the `pthread` library to ensure compatibility across all POSIX-compliant systems.

## 📊 Performance Metrics

When analyzing the output of this system, three primary metrics are considered:
1.  **Wall-Clock Time**: The actual time the user waits for completion.
2.  **User Time**: The amount of time the CPU spent executing the program logic.
3.  **System Time**: The time spent in kernel-level tasks, such as thread creation and context switching.

## 🛡️ Compatibility and Standards

The project adheres to the **ISO C++11 standard** and the **POSIX.1-2008** threading standard, ensuring it remains a reliable tool for systems programming education and performance research on Linux-based platforms.
