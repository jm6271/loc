# Requirements Document

## Introduction

This document specifies the requirements for refactoring the `loc` project's C++ source files to move all method and function implementations out of header files and into their corresponding `.cpp` files. The refactoring enforces proper separation of interface from implementation, reduces compilation coupling, and aligns the project with standard C++ conventions. No behavioral changes are introduced — the existing test suite serves as the correctness oracle.

## Glossary

- **Header**: A `.h` file containing class/struct declarations, type definitions, and function signatures
- **Source**: A `.cpp` file containing method and function definitions (implementations)
- **Declaration**: A function or method signature without a body
- **Definition**: A function or method signature with its full body
- **ODR**: One Definition Rule — a C++ rule requiring that non-inline functions have exactly one definition across all translation units
- **Translation Unit**: A single `.cpp` file and all headers it includes, compiled as a unit
- **Scope Qualifier**: The `ClassName::` prefix required when defining a class method outside the class body
- **constexpr**: A C++ specifier indicating a value or function that can be evaluated at compile time; must remain visible in the header
- **Refactoring**: A structural code transformation that preserves observable behavior

## Requirements

### Requirement 1: Header Files Contain Only Declarations

**User Story:** As a C++ developer, I want header files to contain only class declarations and signatures, so that compilation coupling is minimized and the interface is clearly separated from the implementation.

#### Acceptance Criteria

1. THE Refactoring SHALL remove all non-`constexpr`, non-`inline` method bodies from every header file in scope (`Counter.h`, `LineCounter.h`, `DirectoryScanner.h`, `ExpandGlob.h`, `FileReader.h`)
2. THE Refactoring SHALL retain `#pragma once` and all `#include` directives required by the declarations in each header
3. THE Refactoring SHALL retain all class member variable declarations, nested types, enums, and nested structs in each header
4. THE Refactoring SHALL retain `static constexpr` member definitions in headers, as they must be visible at the call site for the compiler
5. WHEN a method was marked `inline` solely to permit definition in a header, THE Refactoring SHALL remove the `inline` keyword from that method's declaration in the header
6. THE Refactoring SHALL remove `#include` directives from headers that are only required by method implementations and not by the declarations

### Requirement 2: Source Files Contain Full Method Definitions

**User Story:** As a C++ developer, I want source files to contain all method implementations with proper scope qualifiers, so that each translation unit compiles independently and the linker can resolve all symbols.

#### Acceptance Criteria

1. THE Refactoring SHALL add the full definition of every non-`constexpr` method to the corresponding `.cpp` file for each of the five file pairs
2. WHEN a method definition is placed in a `.cpp` file, THE Refactoring SHALL prefix the method name with the `ClassName::` scope qualifier
3. WHEN a method was marked `inline` in the header, THE Refactoring SHALL omit the `inline` keyword from the definition in the `.cpp` file
4. THE Refactoring SHALL add any `#include` directives to the `.cpp` file that are required by the moved implementations but were previously satisfied transitively through the header

### Requirement 3: Special Cases Handled Correctly

**User Story:** As a C++ developer, I want special language constructs handled according to C++ rules, so that the refactored code compiles correctly and obeys the One Definition Rule.

#### Acceptance Criteria

1. THE Refactoring SHALL retain the `is_whitespace` `static constexpr std::array<bool, 256>` member definition inside `FileReader.h`
2. THE Refactoring SHALL retain the `supported_extensions` `static constexpr` array definition inside `DirectoryScanner.h`
3. THE Refactoring SHALL retain the `comma_numpunct` nested struct definition (including its short method bodies) inside `Counter.h`
4. THE Refactoring SHALL retain the `FILE_LANGUAGE` private enum class definition inside `Counter.h`
5. WHEN `to_lower_ascii()` and `normalize_ext()` are moved to `DirectoryScanner.cpp`, THE Refactoring SHALL remove the `inline` keyword from their definitions

### Requirement 4: Build Integrity Preserved

**User Story:** As a developer, I want the project to compile and link cleanly after the refactoring, so that no ODR violations or missing-include errors are introduced.

#### Acceptance Criteria

1. WHEN the refactoring is complete, THE Project SHALL compile without errors or warnings using the existing CMake build configuration
2. WHEN the refactoring is complete, THE Project SHALL link without errors, with no ODR violations caused by duplicate non-inline definitions
3. IF a method body is moved to a `.cpp` file but a required `#include` is missing from that `.cpp`, THEN THE Build SHALL produce a compile error that identifies the missing dependency (this must be resolved before the refactoring is considered complete)
4. WHEN the refactoring is complete, THE Project SHALL produce the same binary behavior as before the refactoring

### Requirement 5: Existing Test Suite Passes

**User Story:** As a developer, I want all existing tests to pass after the refactoring, so that I have confidence no behavior was accidentally changed.

#### Acceptance Criteria

1. WHEN the refactoring is complete, THE Test_Counter test suite SHALL pass without modification
2. WHEN the refactoring is complete, THE Test_CLineCounter test suite SHALL pass without modification
3. WHEN the refactoring is complete, THE Test_FSLineCounter test suite SHALL pass without modification
4. WHEN the refactoring is complete, THE Test_PyLineCounter test suite SHALL pass without modification
5. WHEN the refactoring is complete, THE Test_XmlLineCounter test suite SHALL pass without modification
6. WHEN the refactoring is complete, THE Test_DirectoryScanner test suite SHALL pass without modification
7. WHEN the refactoring is complete, THE Test_ExpandGlob test suite SHALL pass without modification

### Requirement 6: Incremental Refactoring Per File Pair

**User Story:** As a developer, I want each header/source pair to be refactored and verified independently, so that any issues can be isolated to a specific file pair.

#### Acceptance Criteria

1. THE Refactoring SHALL be applied to one header/source pair at a time in the following order: `FileReader`, `LineCounter`, `DirectoryScanner`, `ExpandGlob`, `Counter`
2. WHEN each individual file pair refactoring is complete, THE Project SHALL compile and all tests SHALL pass before proceeding to the next file pair
