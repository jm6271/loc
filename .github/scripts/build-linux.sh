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
cmake --preset=linux-debug ..

# Build the project
cd ../out/build/linux-debug
ninja
cd ../../..
