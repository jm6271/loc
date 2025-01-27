#!/bin/sh

# Clone vcpkg repository
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
cd ..
export VCPKG_ROOT=$(pwd)/vcpkg
export PATH=${VCPKG_ROOT}:${PATH}

# Create build directory and configure
mkdir -p build
cd build
cmake --preset=macos-debug .. -DCMAKE_CXX_COMPILER=$CXX -DCMAKE_C_COMPILER=$CC

# Build the project
cd ../out/build/macos-debug
ninja
cd ../../..
