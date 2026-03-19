# Tasks: remove-cpp-modules

## Task List

- [x] 1 Convert loc.Filesystem module units to header + source files
  - [x] 1.1 Create `DirectoryScanner.h` with `#pragma once`, class declaration, and all inline method bodies moved from `DirectoryScanner.ixx`
  - [x] 1.2 Create `DirectoryScanner.cpp` with `#include "DirectoryScanner.h"` (and any non-inline implementations if extracted)
  - [x] 1.3 Create `ExpandGlob.h` with `#pragma once` and full class definition from `ExpandGlob.ixx`
  - [x] 1.4 Create `ExpandGlob.cpp` with `#include "ExpandGlob.h"`
  - [x] 1.5 Create `FileReader.h` with `#pragma once` and full class definition from `FileReader.ixx`
  - [x] 1.6 Create `FileReader.cpp` with `#include "FileReader.h"`
  - [x] 1.7 Delete the four original `.ixx` files (`DirectoryScanner.ixx`, `ExpandGlob.ixx`, `FileReader.ixx`, `loc.Filesystem.ixx`)

- [x] 2 Convert loc.Counter module units to header + source files
  - [x] 2.1 Create `LineCounter.h` with `#pragma once` and full class definition from `LineCounter.ixx`; replace `import loc.Filesystem;` with `#include "loc.Filesystem/FileReader.h"`
  - [x] 2.2 Create `LineCounter.cpp` with `#include "LineCounter.h"`
  - [x] 2.3 Create `Counter.h` with `#pragma once` and full class definition from `Counter.ixx`; replace `import loc.Filesystem;` with `#include "loc.Filesystem/DirectoryScanner.h"` and `#include "loc.Filesystem/ExpandGlob.h"`, and replace `import :LineCounter;` with `#include "loc.Counter/LineCounter.h"`
  - [x] 2.4 Create `Counter.cpp` with `#include "Counter.h"`
  - [x] 2.5 Delete the three original `.ixx` files (`LineCounter.ixx`, `Counter.ixx`, `loc.Counter.ixx`)

- [x] 3 Update loc/CMakeLists.txt
  - [x] 3.1 Remove the `add_library(loc.Filesystem ...)` target and its `FILE_SET CXX_MODULES` block entirely
  - [x] 3.2 Remove the `add_library(loc.Counter ...)` target and its `FILE_SET CXX_MODULES` block entirely
  - [x] 3.3 Update `add_executable(loc ...)` to include all `.cpp` source files from `loc.Filesystem` and `loc.Counter` directly (e.g. via a `LOC_SOURCES` variable)
  - [x] 3.4 Add `target_include_directories(loc PRIVATE src)` so that `#include "loc.Counter/Counter.h"` and `#include "loc.Filesystem/DirectoryScanner.h"` (etc.) resolve correctly
  - [x] 3.5 Remove `loc.Filesystem` and `loc.Counter` from `target_link_libraries(loc ...)` since those targets no longer exist
  - [x] 3.6 Optionally lower the minimum CMake version from 3.30 to 3.14

- [x] 4 Update loc.tests/CMakeLists.txt
  - [x] 4.1 Remove the `/experimental:module`, `-fmodules-ts`, and `-fmodules` compiler option blocks
  - [x] 4.2 Add the same `.cpp` source files from `loc/src/` to the `add_executable(loc.tests ...)` call (using the same `LOC_SOURCES` list or equivalent relative paths)
  - [x] 4.3 Replace `target_link_libraries(loc.tests ... loc.Filesystem loc.Counter)` — remove those two targets (they no longer exist); keep `Catch2::Catch2WithMain`
  - [x] 4.4 Add `target_include_directories(loc.tests PRIVATE ../loc/src)` so test files can resolve `#include "loc.Counter/Counter.h"` and `#include "loc.Filesystem/DirectoryScanner.h"` (etc.)
  - [x] 4.5 Remove the `include_directories("../loc")` directive if it is superseded by the new `target_include_directories` call

- [x] 5 Update test source files
  - [x] 5.1 In `Test_Counter.cpp`: replace `import loc.Counter;` with `#include "loc.Counter/Counter.h"`
  - [x] 5.2 In `Test_CLineCounter.cpp`: replace `import loc.Counter;` with `#include "loc.Counter/Counter.h"`
  - [x] 5.3 In `Test_DirectoryScanner.cpp`: replace `import loc.Filesystem;` with `#include "loc.Filesystem/DirectoryScanner.h"`
  - [x] 5.4 In `Test_ExpandGlob.cpp`: replace `import loc.Filesystem;` with `#include "loc.Filesystem/ExpandGlob.h"`
  - [x] 5.5 In `Test_FSLineCounter.cpp`: replace `import loc.Counter;` with `#include "loc.Counter/Counter.h"`
  - [x] 5.6 In `Test_PyLineCounter.cpp`: replace `import loc.Counter;` with `#include "loc.Counter/Counter.h"`
  - [x] 5.7 In `Test_XmlLineCounter.cpp`: replace `import loc.Counter;` with `#include "loc.Counter/Counter.h"`

- [x] 6 Update main.cpp
  - [x] 6.1 Replace `import loc.Counter;` with `#include "loc.Counter/Counter.h"` in `loc/src/main.cpp`

- [x] 7 Verify build and tests
  - [x] 7.1 Confirm no `.ixx` files remain in the repository
  - [x] 7.2 Build the full project (`cmake --build`) and confirm zero new errors or warnings
  - [x] 7.3 Run `ctest` and confirm all tests pass
