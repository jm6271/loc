# Implementation Plan: move-impl-to-cpp

## Overview

Refactor each of the five header/source pairs to move all non-`constexpr` method bodies out of headers and into their corresponding `.cpp` files. Each file pair is refactored and verified independently in the order specified by Requirement 6: `FileReader` → `LineCounter` → `DirectoryScanner` → `ExpandGlob` → `Counter`.

## Tasks

- [x] 1. Refactor FileReader
  - [x] 1.1 Move `FileReader` method bodies to `FileReader.cpp`
    - Move `ReadFile()` and `trim()` definitions from `FileReader.h` to `FileReader.cpp` with `FileReader::` scope qualifiers
    - Add `#include <fstream>` and `#include <iostream>` to `FileReader.cpp` (remove them from the header)
    - Retain `is_whitespace` `static constexpr` array definition in `FileReader.h`
    - Keep only the method declarations (signatures) in `FileReader.h`
    - _Requirements: 1.1, 1.4, 1.5, 1.6, 2.1, 2.2, 2.4, 3.1_

  - [x] 1.2 Verify build and tests pass for FileReader
    - Build the project and confirm no compile or link errors
    - Run `loc.tests` and confirm all test suites pass
    - _Requirements: 4.1, 4.2, 5.1, 5.2, 5.3, 5.4, 5.5, 6.2_

- [x] 2. Refactor LineCounter
  - [x] 2.1 Move `LineCounter` method bodies to `LineCounter.cpp`
    - Move `CountLines()` and `StrContains()` definitions from `LineCounter.h` to `LineCounter.cpp` with `LineCounter::` scope qualifiers
    - Ensure `LineCounter.cpp` includes `"LineCounter.h"` (already present) and any headers needed by the implementation
    - Keep only the method declarations in `LineCounter.h`
    - _Requirements: 1.1, 1.5, 1.6, 2.1, 2.2, 2.4_

  - [x] 2.2 Verify build and tests pass for LineCounter
    - Build the project and confirm no compile or link errors
    - Run `loc.tests` and confirm all test suites pass
    - _Requirements: 4.1, 4.2, 5.2, 5.3, 5.4, 5.5, 6.2_

- [x] 3. Refactor DirectoryScanner
  - [x] 3.1 Move `DirectoryScanner` method bodies to `DirectoryScanner.cpp`
    - Move `Scan()`, `to_lower_ascii()`, and `normalize_ext()` definitions from `DirectoryScanner.h` to `DirectoryScanner.cpp` with `DirectoryScanner::` scope qualifiers
    - Remove the `inline` keyword from `to_lower_ascii()` and `normalize_ext()` in the `.cpp` definitions
    - Retain `supported_extensions` `static constexpr` array definition in `DirectoryScanner.h`
    - Remove `#include <algorithm>`, `<cctype>`, `<system_error>`, and `<unordered_set>` from the header if they are only needed by the implementation; add them to `DirectoryScanner.cpp`
    - Keep only the method declarations in `DirectoryScanner.h`
    - _Requirements: 1.1, 1.5, 1.6, 2.1, 2.2, 2.3, 2.4, 3.2, 3.5_

  - [x] 3.2 Verify build and tests pass for DirectoryScanner
    - Build the project and confirm no compile or link errors
    - Run `loc.tests` and confirm all test suites pass
    - _Requirements: 4.1, 4.2, 5.6, 6.2_

- [x] 4. Refactor ExpandGlob
  - [x] 4.1 Move `ExpandGlob` method bodies to `ExpandGlob.cpp`
    - Move `expand_glob()`, `glob_to_regex()`, and `is_in_hidden_directory()` definitions from `ExpandGlob.h` to `ExpandGlob.cpp` with `ExpandGlob::` scope qualifiers
    - Move `#include <regex>` and `#include <iostream>` to `ExpandGlob.cpp` if not needed by declarations in the header
    - Keep only the method declarations in `ExpandGlob.h`
    - _Requirements: 1.1, 1.5, 1.6, 2.1, 2.2, 2.4_

  - [x] 4.2 Verify build and tests pass for ExpandGlob
    - Build the project and confirm no compile or link errors
    - Run `loc.tests` and confirm all test suites pass
    - _Requirements: 4.1, 4.2, 5.7, 6.2_

- [x] 5. Checkpoint — verify incremental state
  - Ensure all tests pass after the first four file pairs are complete, ask the user if questions arise.

- [x] 6. Refactor Counter
  - [x] 6.1 Move `Counter` method bodies to `Counter.cpp`
    - Move all non-`constexpr` method definitions (`Counter` constructors, `Count()`, `PrintLanguageBreakdown()`, `IsDirectory()`, `CountFile()`, `GetFileLanguage()`, `CounterWorker()`, `isFileInDirectory()`, `expandAllGlobsInPaths()`, `GetNextPaths()`) from `Counter.h` to `Counter.cpp` with `Counter::` scope qualifiers
    - Retain `comma_numpunct` nested struct definition (including its short method bodies) in `Counter.h`
    - Retain `FILE_LANGUAGE` private enum class definition in `Counter.h`
    - Move implementation-only includes (`<fstream>`, `<sstream>`, `<future>`, `<iostream>`, `<queue>`, `<iomanip>`) to `Counter.cpp` if not needed by declarations
    - Keep only the method declarations in `Counter.h`
    - _Requirements: 1.1, 1.3, 1.4, 1.5, 1.6, 2.1, 2.2, 2.4, 3.3, 3.4_

  - [x] 6.2 Verify build and tests pass for Counter
    - Build the project and confirm no compile or link errors
    - Run `loc.tests` and confirm all test suites pass
    - _Requirements: 4.1, 4.2, 5.1, 6.2_

- [x] 7. Final checkpoint — full test suite
  - Ensure all tests pass across all five refactored file pairs, ask the user if questions arise.

## Notes

- Tasks marked with `*` are optional and can be skipped for faster MVP
- Each task references specific requirements for traceability
- `static constexpr` members (`is_whitespace`, `supported_extensions`) must remain in their headers — do not move them
- The `comma_numpunct` nested struct and `FILE_LANGUAGE` enum stay in `Counter.h` as part of the class definition
- No behavioral changes are introduced; the existing test suite in `loc.tests/` is the correctness oracle
