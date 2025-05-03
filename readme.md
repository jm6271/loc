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

Any C-style language should work, and the program uses 
the C line counter for any file extension not recognized.

## Installation

1. Download the latest release binary for your platform from the GitHub Releases
2. Extract the zip to a location on your path

## Building from source

Requires a C++ compiler, CMake, and ninja.

### Windows:

``` Powershell
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

``` Bash
# clone the repo
git clone https://github.com/jm6271/loc.git
cd loc

# setup vcpkg
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg..sh
cd ..
export VCPKG_ROOT=$(pwd)/vcpkg
export PATH=${VCPKG_ROOT}:${PATH}

# Create build directory and configure
mkdir -p build
cd build
cmake --preset=linux-release ..

# Build the project
cd ../out/build/linux-release
ninja
cd ../../..
```

## Usage

```loc [options] path1 path2 ...```

### Options

```-h [ --help ]``` - Display help

```-j [ --jobs ]``` - Number of threads to use. Default is number of CPU cores available

```pattern``` - Semicolon-separated list of file extensions to search for

```path``` - Path to file. Can be a wildcard pattern

### Example

To count the lines of code in the ```loc``` codebase from 
the toplevel directory running on 10 threads, use this command:

```
loc -j 10 ./loc/*.cpp ./loc/include/*.h ./loc.tests/*.cpp
```

## Speed

```loc``` is designed to be as fast as possible using multi-threading.

Counting the total lines of code in the Boost C++ libraries
(over 4 million lines of code) takes about 5.1 seconds.
