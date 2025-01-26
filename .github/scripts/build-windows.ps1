# Clone vcpkg repository
git clone https://github.com/microsoft/vcpkg.git
Set-Location -Path vcpkg
./bootstrap-vcpkg.bat
Set-Location -Path ..

# Set environment variables
$env:VCPKG_ROOT = (Get-Location).Path + "\vcpkg"
$env:PATH = $env:VCPKG_ROOT + ";" + $env:PATH

# Create build directory and configure
New-Item -ItemType Directory -Path build -Force
Set-Location -Path build
cmake --preset=x64-debug --build ..
