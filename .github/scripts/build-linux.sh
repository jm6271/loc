git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
cd ..
export VCPKG_ROOT=$(pwd)/vcpkg
export PATH=$(VCPKG_ROOT):$(PATH)

sudo apt install ninja-build -y

mkdir -p build
cd build
export VCPKG_ROOT="$VCPKG_ROOT"
cmake --preset=linux-debug ..
cd ../out/build/linux-debug
ninja
cd ../../..
