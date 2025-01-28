#!/bin/sh

# Clone vcpkg repository
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
cd ..
export VCPKG_ROOT=$(pwd)/vcpkg
export PATH=${VCPKG_ROOT}:${PATH}
export CC=gcc-11
export CXX=g++-11

# Create build directory and configure
mkdir -p build
cd build
cmake --preset=linux-release ..

# Build the project
cd ../out/build/linux-release
ninja
cd ../../..
