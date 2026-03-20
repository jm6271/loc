# Requirements: remove-cpp-modules

## Introduction

This document captures the functional and structural requirements for converting the `loc` project from C++20 named modules (`.ixx` files) to traditional header + source files (`.h` / `.cpp`). The refactor must preserve all existing runtime behaviour and test coverage.

After conversion, the separate `loc.Filesystem` and `loc.Counter` CMake library targets are **removed entirely**. All source files are compiled directly into the `loc` executable and directly into the `loc.tests` executable — there are no intermediate static libraries.

---

## Requirements

### Requirement 1: Convert loc.Filesystem module units

**User Story**: As a developer, I want the `loc.Filesystem` sources to use standard header + source files so that any C++20-capable compiler and IDE can build and navigate the code without module support.

#### Acceptance Criteria

1.1 `DirectoryScanner.ixx` is replaced by `DirectoryScanner.h` and `DirectoryScanner.cpp` in `loc/src/loc.Filesystem/`.
1.2 `ExpandGlob.ixx` is replaced by `ExpandGlob.h` and `ExpandGlob.cpp` in `loc/src/loc.Filesystem/`.
1.3 `FileReader.ixx` is replaced by `FileReader.h` and `FileReader.cpp` in `loc/src/loc.Filesystem/`.
1.4 Every new header file contains `#pragma once` as its first non-comment line.
1.5 No `export module`, `import`, or `module;` keywords appear in any file under `loc/src/loc.Filesystem/`.
1.6 The aggregator `loc.Filesystem.ixx` is deleted; no replacement umbrella header is created. Consumers include individual headers directly (e.g. `#include "loc.Filesystem/DirectoryScanner.h"`).

---

### Requirement 2: Convert loc.Counter module units

**User Story**: As a developer, I want the `loc.Counter` sources to use standard header + source files for the same portability and tooling reasons.

#### Acceptance Criteria

2.1 `LineCounter.ixx` is replaced by `LineCounter.h` and `LineCounter.cpp` in `loc/src/loc.Counter/`.
2.2 `Counter.ixx` is replaced by `Counter.h` and `Counter.cpp` in `loc/src/loc.Counter/`.
2.3 Every new header file contains `#pragma once` as its first non-comment line.
2.4 No `export module`, `import`, or `module;` keywords appear in any file under `loc/src/loc.Counter/`.
2.5 The aggregator `loc.Counter.ixx` is deleted; no replacement umbrella header is created. Consumers include individual headers directly (e.g. `#include "loc.Counter/Counter.h"`).
2.6 `LineCounter.h` includes `#include "loc.Filesystem/FileReader.h"` instead of `import loc.Filesystem;`.
2.7 `Counter.h` includes `#include "loc.Filesystem/DirectoryScanner.h"`, `#include "loc.Filesystem/ExpandGlob.h"`, and `#include "loc.Counter/LineCounter.h"` instead of `import loc.Filesystem;` and `import :LineCounter;`.

---

### Requirement 3: Update loc/CMakeLists.txt

**User Story**: As a developer, I want the CMake build definition to reflect the new file layout so the project builds cleanly without module-specific CMake features or separate library targets.

#### Acceptance Criteria

3.1 The `loc.Filesystem` static library target (`add_library(loc.Filesystem ...)`) is removed entirely.
3.2 The `loc.Counter` static library target (`add_library(loc.Counter ...)`) is removed entirely.
3.3 The `loc` executable target is updated to compile all `.cpp` source files directly via `target_sources` or by listing them in `add_executable`.
3.4 The `loc` executable target uses `target_include_directories(loc PRIVATE src)` so that `#include "loc.Counter/Counter.h"` and `#include "loc.Filesystem/DirectoryScanner.h"` (etc.) resolve correctly.
3.5 The minimum required CMake version may be lowered (e.g. to 3.14) since `FILE_SET CXX_MODULES` required 3.28; the version must remain compatible with all other CMake features used.

---

### Requirement 4: Update loc.tests/CMakeLists.txt

**User Story**: As a developer, I want the test build to compile all source files directly without module-specific compiler flags or links to separate library targets.

#### Acceptance Criteria

4.1 The `/experimental:module`, `-fmodules-ts`, and `-fmodules` compiler option blocks are removed from `loc.tests/CMakeLists.txt`.
4.2 The `loc.tests` target no longer links against `loc.Filesystem` or `loc.Counter` (those targets no longer exist).
4.3 The `loc.tests` target compiles the same `.cpp` source files from `loc/src/` directly (listed in `add_executable` or via a shared variable).
4.4 `target_include_directories(loc.tests PRIVATE ../loc/src)` (or equivalent) is set so test files can resolve `#include "loc.Counter/Counter.h"` and `#include "loc.Filesystem/DirectoryScanner.h"` (etc.).

---

### Requirement 5: Update test source files

**User Story**: As a developer, I want the test files to use `#include` directives instead of `import` statements so they compile with the traditional model.

#### Acceptance Criteria

5.1 `Test_Counter.cpp`, `Test_CLineCounter.cpp`, `Test_FSLineCounter.cpp`, `Test_PyLineCounter.cpp`, and `Test_XmlLineCounter.cpp` each replace `import loc.Counter;` with `#include "loc.Counter/Counter.h"`.
5.2 `Test_DirectoryScanner.cpp` replaces `import loc.Filesystem;` with `#include "loc.Filesystem/DirectoryScanner.h"`.
5.3 `Test_ExpandGlob.cpp` replaces `import loc.Filesystem;` with `#include "loc.Filesystem/ExpandGlob.h"`.
5.4 No `import` statements remain in any file under `loc.tests/`.

---

### Requirement 6: Update main.cpp

**User Story**: As a developer, I want the main executable entry point to use `#include` instead of `import`.

#### Acceptance Criteria

6.1 `import loc.Counter;` in `loc/src/main.cpp` is replaced with `#include "loc.Counter/Counter.h"`.
6.2 No `import` statements remain in `loc/src/main.cpp`.

---

### Requirement 7: Build and test correctness

**User Story**: As a developer, I want the refactored project to build cleanly and all existing tests to pass so that no behaviour is changed.

#### Acceptance Criteria

7.1 The project builds without errors or warnings introduced by the refactor (i.e. no new warnings beyond those present before the refactor).
7.2 All Catch2 tests that passed before the refactor continue to pass after it (`ctest` reports 100% pass rate).
7.3 The `loc` executable links and runs correctly, producing the same output as before for equivalent inputs.
7.4 No `.ixx` files remain anywhere in the repository after the refactor is complete.
