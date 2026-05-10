# 🛡️ Security Policy: Safeguarding Time

At the core of the **Typical vs Threaded Program Time Tester** is a respect for the user's environment and resources. While this is a benchmarking utility, we treat the security of your system as a priority.

## 🏹 Supported Versions

We only provide security updates for the latest stable release.

| Version | Supported            |
| ------- | -------------------- |
| Latest  | ✅ Supported         |
| < 1.0.0 | ❌ End of Life (EOL) |

## 🛡️ Our Commitment

Since this project interacts with system threads and high-recursion logic, we ensure:

1. **No Memory Leaks**: We use modern C++ smart pointers to prevent resource exhaustion.
2. **Minimal Privileges**: The engine requires no elevated permissions to run.
3. **Transparent Logic**: The source code is modular and open for audit.

## 🚨 Reporting a Vulnerability

If you discover a security vulnerability, please do not open a public issue. Instead:

1. Reach out via a private message or email to the maintainers.
2. Provide a detailed description of the vulnerability and a proof-of-concept if possible.
3. Allow us time to address the issue before public disclosure.

_Thank you for helping us keep the pulse of parallelism secure._
