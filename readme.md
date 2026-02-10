# loc

```loc``` is a tool for counting the lines of code in a
programming project. It is written in C++ and designed to be very fast.

## Suported languages

```loc``` supports the following languages:

- C
- C#
- C++
- F#
- Go
- HTML
- Java
- JavaScript
- JSX/TSX
- Kotlin
- PowerShell
- Python
- Ruby
- Rust
- Shell
- TypeScript
- XAML
- XML

## Installation

1. Download the latest release binary for your platform from the GitHub Releases
2. Extract the zip to a location on your path

## Building from source

Requires a C++ compiler, CMake, and ninja.

### Windows:

``` Powershell
# Powershell

# clone the repo
git clone https://github.com/jm6271/loc.git
cd loc

# setup vcpkg
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat
cd ..
$env:VCPKG_ROOT = (Get-Location).Path + "\vcpkg"
$env:PATH = $env:VCPKG_ROOT + ";" + $env:PATH

# build loc
cmake --preset=x64-release # replace preset with x86-release for 32-bit
cd out/build/x64-release # or x86-release
ninja

# release binary is located at out/build/x64-release/loc/loc.exe
```

### Linux:

**Note:** You may need to use clang, gcc might not work.

``` Bash
# Bash

# clone the repo
git clone https://github.com/jm6271/loc.git
cd loc

# setup vcpkg
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
cd ..
export VCPKG_ROOT=$(pwd)/vcpkg
export PATH=${VCPKG_ROOT}:${PATH}

# configure
cmake --preset=linux-release . -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++

# Build the project
cd out/build/linux-release
ninja

# Run the tests
cd loc.tests
./loc.tests

# release binary is located at out/build/linux-release/loc/loc
```

## Usage

```loc [options] [paths...]```

### Options

```-h [ --help ]``` - Display help

```-j [ --jobs ]``` - Number of threads to use. Default is number of CPU cores available

```-v [ --version ]``` - Display version

```--include-hidden``` - Include hidden files and files in build directory (ignored by default)

`-i,--ignore TEXT ...` Directories to ignore (relative to the provided directory to search)

### Paths

The list of paths can be a list of paths to any files or directories. If any directories are specified,
The application will scan the directory and its subdirectories for any files with supported file extensions.
If any file paths are provided directly, the application will skip over them if the extension is not supported.

### Example

To count the lines of code in the ```loc``` codebase from 
the toplevel directory use this command:

```
loc .
```
