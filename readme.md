# loc

```loc``` is a tool for counting the lines of code in a
programming project. It is written in C++ and designed to be very fast.

## Suported languages

```loc``` supports the following languages:

- C
- C++
- C#
- F#
- Python

Any language that uses C-style comments should work. If the program
doesn't recognize a file extension, it uses the C line counter.

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
mkdir build
cd build
cmake --preset=x64-release build .. # replace preset with x86-release for 32-bit
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

```loc [options] subcommand```

### Options

```-h [ --help ]``` - Display help

```-j [ --jobs ]``` - Number of threads to use. Default is number of CPU cores available

#### Subcommands:

##### ```files```

Count the lines of code in individual files

Usage: ```loc [options] files [options] path1 path2 ...```

##### ```dir```

Count the lines of code in files with the specified
extensions in a directory

Usage: ```loc [options] dir [options] directory```

Options:

`--cpp` Search for files with C++ extensions

`--cs` Search for files with C# extensions

`--py` Search for files with Python extensions

`--fs` Search for files with F# extensions

`--c` Search for files with C extensions

`-e,--extention TEXT ...`  Other extensions to pass in.
For example: `loc dir -e .cpp -e .h .`

`-i,--ignore TEXT ...` Directories to ignore (relative to the provided directory to search)

`--include-generated` Include build files and generated files in directories like `obj/`, `out/`, `.git/`, and `bin/`

### Example

To count the lines of code in the ```loc``` codebase from 
the toplevel directory running on 10 threads, use this command:

```
loc -j 10 files ./loc/*.cpp ./loc/include/*.h ./loc.tests/*.cpp
```

Or use the directory command:

```
loc -j 10 dir --cpp .
```

## Speed

```loc``` is designed to be as fast as possible using multi-threading.

Counting the total lines of code in the Boost C++ libraries
(over 4 million lines of code) takes about 2.5 seconds on 12 cores.
