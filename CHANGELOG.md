# CHANGELOG

## 2.0.0 (2026-02-10)

### Major Changes

- Reorganized project structure:
  - Headers moved to `include/`
  - Implementation moved to `src/`
  - Examples moved to `examples/`
  - Tests moved to `tests/`

- Added CMake build system:
  - Full CMake support with install rules
  - pkg-config (.pc) file generation
  - CMake package config files for downstream projects

- Documentation improvements:
  - Updated LIBRARY.md with Threat Model section
  - Added Conceptual Modes documentation (SYSTEM_ENTROPY, CHAOTIC_FEEDBACK)
  - Removed emoji from all docs for ASCII compatibility
  - Cleaned up QUICK_START.md and README.md

- License update:
  - Changed from RPL-1.0 to Apache-2.0
  - Added explicit liability clause for commercial misuse

- Code quality:
  - Added Doxygen-compatible comments to public API
  - Improved header file documentation
  - Better error handling and validation

### API Changes

- Header include path changes:
  - From: `#include "agle.h"`
  - To:   `#include "agle.h"` (same, but now in include/)

- CMake-based projects now use:
  - `find_package(AGLE REQUIRED)`
  - `target_link_libraries(target PRIVATE AGLE::agle)`

### Build System

- Default build now CMake
- Makefile retained for backward compatibility
- Added: debug, release, lint, format targets
- Improved: install, uninstall targets

### Dependencies

- No new dependencies
- OpenSSL 3.0+ (already required)
- CMake 3.16+ (for cmake build)

## 1.0.0 (2025-2025)

### Initial Release

- Core RNG functionality
- Password generation
- SHAKE256 hashing
- Key derivation (KDF)
- Session tokens
- Utility functions
