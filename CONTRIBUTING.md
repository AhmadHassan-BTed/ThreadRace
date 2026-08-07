# Contributing: A Symphony of Human Effort

First off, thank you for considering contributing to **ThreadRace** and its **Native C++ Visualizer Suite**. It’s contributions like yours that bridge abstract recursive computer science with visual art and engineering precision.

---

## How You Can Help

We welcome contributions across all areas of the project:

- **New Benchmark Tasks**: Implement new `ITask` algorithms (e.g., Matrix Inversion, N-Queens, Prime Sieves, FFT).
- **New Execution Strategies**: Add novel concurrency models to `IExecutionStrategy` (e.g., Work-Stealing Pools, SIMD Vectorization).
- **Visualizer Render Engines**: Build custom 2D/3D renderers in `src/visualizer/` using OpenGL & SDL2.
- **Audio Sonification & Shaders**: Enhance real-time PCM audio synthesis or GL particle effects.
- **Documentation & Benchmarking**: Improve clarity, add diagrams, or refine telemetry reporting.

---

## Your Development Journey

1. **Fork & Clone**: Clone the repository to your local machine.
2. **Install Dependencies**: Install `libsdl2-dev` and `libgl1-mesa-dev` as detailed in [docs/development.md](file:///home/leech/Projects/ThreadRace/docs/development.md).
3. **Build Target Binaries**: Execute `make all` to compile `./bin/time_tester` and `./bin/ackermann_visualizer`.
4. **Create Feature Branch**: Name your branch descriptively (e.g., `feat/add-work-stealing-strategy` or `feat/3d-raymarching-mode`).
5. **Implement & Test**: Run `./bin/time_tester` and `./bin/ackermann_visualizer` to verify stability and zero regressions.
6. **Submit Pull Request**: Open a PR with details of your performance findings or visualizer enhancements.

---

## Coding Standards & Guidelines

- **Decoupled Design**: Follow the Strategy & Component design patterns. Keep renderer logic separate from stack state machine evaluation.
- **Modern C++11 Compatibility**: Maintain C++11 standard compliance without external framework bloat.
- **Documentation Integrity**: Reflect major architecture changes in `README.md` and `docs/architecture.md`.

---

## Community & Discussion

Have ideas for new visualizer modes or concurrency benchmarks? Open an **Issue** or submit a **Pull Request**. We build together!

---

_Time is non-renewable, but through digital collaboration, we expand human capability._
