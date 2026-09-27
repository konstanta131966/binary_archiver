# Binary Archiver (`arc`)

A lightweight, modern C++20 binary file archiver and unpacker utility inspired by POSIX `tar` and PKZIP. 
Built with raw binary stream I/O, custom binary header specifications, automated Catch2 unit testing(pretty basic), and CMake architecture.

---

## Features

- **Custom Binary Specification (`.arc`):** Implements an End-of-Central-Directory (EOCD) format for $O(1)$ random-access metadata inspection thus avoiding having to read the whole-file payloads.
- **Raw Stream I/O:** Zero-heap chunked streaming via 64 KB buffers, avoiding RAM exhaustion on large files.
- **Zero Struct Padding:** Aligned binary layouts using `#pragma pack(push, 1)` and compile-time `static_assert` layout guarantees.
- **POSIX Fidelity:** Preserves file permissions and nested directory hierarchies via C++20 `<filesystem>`.
- **Defensive Safeguards:** Built-in loop-prevention against self-packing (`fs::equivalent`) and fixed-width path bounds checking.
- **Automated Testing:** Automated roundtrip test leveraging Catch2 v3 (via CMake's `FetchContent`.)

---

## Binary Format Specification

The `.arc` binary layout isolates payloads from metadata using a trailing Central Directory:

```text
+--------------------------------------------------------------------+
| Payload Data (File 1 raw bytes)                                   |
| Payload Data (File 2 raw bytes)                                   |
| ...                                                                |
+--------------------------------------------------------------------+
| Central Directory (Table of Contents)                              |
|   - FileEntry 1 [path (256B) | offset (8B) | size (8B) | perm (4B)]|
|   - FileEntry 2 [path (256B) | offset (8B) | size (8B) | perm (4B)]|
+--------------------------------------------------------------------+
| EndRecord (Trailer - 22 bytes)                                     |
|   - Magic Number: 0x444E4541 ("AEND", 4B)                          |
|   - Format Version: 1 (2B)                                         |
|   - Directory Offset: byte position of TOC (8B)                    |
|   - Entry Count: number of FileEntry records (8B)                  |
+--------------------------------------------------------------------+
```

## Project Structure
```text
├── CMakeLists.txt              # Build configuration and Catch2 FetchContent
├── include/
│   ├── Archiver.hpp            # Public interface (pack, list, extract)
│   └── ArchiverFormat.hpp      # Binary protocol, byte constants, and packed structs
├── src/
│   ├── Archiver.cpp            # Core streaming and traversal implementation
│   └── main.cpp                # CLI entry point and argument dispatcher
└── tests/
    └── test_main.cpp           # Catch2 roundtrip tests in /tmp sandbox
```
## Build Instructions

~ Requirements:
 C++ compiler with C++20 support (GCC $\ge$ 11 or Clang $\ge$ 13), CMake $\ge$ 3.20, Git (for fetching Catch2) ,Linux (tested on Fedora)

1. Build Library & Executables
Bash
```
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```
2. Run Automated Unit Tests
Bash
```
ctest --test-dir build --output-on-failure
```
3. Optional: Install to User $PATH

To use arc like standard shell utilities (ls, tar):
Bash
```
cmake -B build -S . -DCMAKE_INSTALL_PREFIX=$HOME/.local
cmake --build build
cmake --install build
```
Ensure ~/.local/bin is in your $PATH.

## Usage Examples
1. Packing Files and Folders

Pack one or more files and folders into an archive:
```
Syntax: arc pack <archive.arc> <input_paths...>
arc pack my_bundle.arc document.txt assets/ src/
```
2. Inspecting Archive Contents

List all archived entries without extracting payloads:
```
Syntax: arc list <archive.arc>
arc list my_bundle.arc
```
Output:
```

Size (B)     Path
----------------------------------------
1420         document.txt
204800       assets/logo.png
4521         src/main.cpp
```
3. Extracting Archives

Extract all files and restore their relative hierarchy and POSIX permissions:
```
Syntax: arc extract <archive.arc> <destination_dir>
arc extract my_bundle.arc ./output_dir
```






