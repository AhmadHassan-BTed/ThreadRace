# Development & Build Workflow

This guide outlines the standard development cycle, build system, prerequisites, and testing protocols for **ThreadRace** and the **Native C++ Visualizer Suite**.

---

## System Prerequisites & Dependencies

To build and run both the CLI timing suite and the native OpenGL visualizer, ensure your POSIX environment (Linux, WSL2, or macOS) has the required tools and graphics/sound dev libraries installed:

```bash
# Ubuntu / Debian
sudo apt-get update
sudo apt-get install -y build-essential g++ make libsdl2-dev libgl1-mesa-dev

# Arch Linux
sudo pacman -S base-devel gcc make sdl2 mesa

# Fedora / RHEL
sudo dnf install gcc-c++ make SDL2-devel mesa-libGL-devel
```

---

## Build System Reference

We use a standard POSIX `Makefile` supporting incremental compilation (`-O2` production optimizations) and multi-target linking.

| Command | Action | Output Binaries |
| :--- | :--- | :--- |
| `make` / `make all` | Compiles core benchmark and C++ OpenGL visualizer targets. | `bin/time_tester`, `bin/ackermann_visualizer` |
| `./bin/time_tester [1\|2]` | Runs CLI performance telemetry (Mode 1: Sequential, Mode 2: Threaded). | CLI Terminal Telemetry |
| `./bin/ackermann_visualizer [m] [n]` | Launches 60 FPS hardware-accelerated C++ visualizer suite. | Native SDL2/OpenGL Window |
| `make test` | Executes regression benchmark checks. | Terminal Test Verification |
| `make clean` | Purges compiled object (`obj/`) and binary (`bin/`) directories. | Cleaned Workspace |

---

## Testing & Verification Protocols

When contributing code changes to either the core telemetry benchmark or the visualizer rendering engines, follow these protocols:

### 1. Functional Integrity Validation
- **CLI Benchmark Verification**: Verify that running `./bin/time_tester 1` completes deterministically and $A(4, 1)$ outputs `65533`.
- **Visualizer Stack Verification**: Run `./bin/ackermann_visualizer 3 3` and verify the simulation completes after 2,432 steps with clean cleanup on exit (`ESC` / `Q`).

### 2. Interactive Visualizer Suite Protocol
Verify all interactive render modes and controls in `./bin/ackermann_visualizer`:
- **Mode Transitions**: Press `1` (Quantum Tree), `2` (3D Landscape), `3` (Spiral Orbit), and `4` (Race Arena) to ensure smooth mode switches.
- **Simulation Control**: Test pause (`SPACE`/`P`), single-step forward (`S`), parameter adjustments (`[`/`]`, `-`/`+`), and simulation reset (`R`).
- **Audio & Optics**: Toggle audio sonification (`M`) to verify audio callback synthesis and toggle HUD (`H`) for demystifier trajectory text overlays.
- **Camera Dynamics**: Pan via mouse drag and zoom in/out (`I`/`O` or scroll wheel) to verify viewport transformations.

---

## Quality & Documentation Standards

- **Code Formatting**: Follow `.editorconfig` rules (4-space indentation, LF line endings, clean modular C++11 code).
- **Architecture Synchronization**: Any modification to `ITask`, `IExecutionStrategy`, `AckermannStackEngine`, or renderers must be updated in `docs/architecture.md`.
- **Commit Conventions**: Use descriptive commit tags (e.g., `feat: add race arena renderer mode 4`, `docs: update visualizer screenshots`).

---

## CI/CD Automation Pipeline

On every Push or Pull Request to `main`, GitHub Actions (`.github/workflows/ci.yml`) automatically:
1. Provisions an Ubuntu runner.
2. Installs `build-essential`.
3. Runs `make` to verify clean compilation of all binaries.
4. Executes `make test` to validate regression standards.
