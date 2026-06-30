# Development Workflow

This guide outlines the standard development cycle for the **Typical vs Threaded Program Time Tester**. 

##  Setup

Ensure you have a POSIX-compliant environment (Linux, macOS, or WSL2) with `g++` and `make` installed.

##  Build System

We use a standard `Makefile` to ensure consistent builds across different environments.

| Command | Action |
| :--- | :--- |
| `make` | Compiles the `time_tester` binary with `-O2` optimizations. |
| `make test` | Executes a standard benchmark run for both modes. |
| `make clean` | Removes compiled binaries and temporary objects. |

##  Testing Protocol

When contributing code changes (especially to the `Ackermann` core or the `pthread` logic), follow these steps:

1.  **Functional Validation**: Verify that `Ackermann(4, 1)` still returns `65533`.
2.  **Concurrency Stress Test**: Run Mode 2 with a high number of iterations to check for race conditions or thread exhaustion.
3.  **Benchmarking**: Use `make test` and compare against the baseline screenshots in `assets/`.

##  Quality Standards

- **Code Style**: Adhere to the `.editorconfig` settings (4-space indentation, LF line endings).
- **Documentation**: Any change to the core logic must be reflected in `docs/architecture.md`.
- **Commits**: Use descriptive commit messages (e.g., `feat: add support for C++11 threads`).

##  CI/CD Pipeline

On every Push and Pull Request to the `main` branch, a GitHub Action (`.github/workflows/ci.yml`) is triggered to:
1.  Initialize a clean Ubuntu environment.
2.  Install `build-essential`.
3.  Execute `make` to verify compilation.
4.  Execute `make test` to verify functional integrity.
