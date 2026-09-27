# Binary Archiver (`arc`)

A lightweight, modern C++20 binary file archiver and unpacker utility inspired by POSIX `tar` and PKZIP. 
Built with raw binary stream I/O, custom binary header specifications, automated Catch2 unit testing(pretty basic), and modular CMake architecture.

---

## Features

- **Custom Binary Specification (`.arc`):** Implements an End-of-Central-Directory (EOCD) format for $O(1)$ random-access metadata inspection thus avoiding having to read the whole-file payloads.
- **Raw Stream I/O:** Zero-heap chunked streaming via 64 KB buffers, avoiding RAM exhaustion on large files.
- **Zero Struct Padding:** Strictly aligned binary layouts using `#pragma pack(push, 1)` and compile-time `static_assert` layout guarantees.
- **POSIX Fidelity:** Preserves file permissions and nested directory hierarchies via C++20 `<filesystem>`.
- **Defensive Safeguards:** Built-in loop-prevention against self-packing (`fs::equivalent`) and fixed-width path bounds checking.
- **Automated Testing:** Automated roundtrip test leveraging Catch2 v3 integrated via CMake's `FetchContent`.

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
