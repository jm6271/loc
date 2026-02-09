cmake_minimum_required(VERSION 3.30)
project(loc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.28)
  cmake_policy(SET CMP0144 NEW)
  cmake_policy(SET CMP0167 NEW)
endif()

find_package(CLI11 CONFIG REQUIRED)

# Filesystem module target
add_library(loc.Filesystem STATIC)
target_sources(loc.Filesystem
  PUBLIC
    FILE_SET CXX_MODULES FILES
      src/loc.Filesystem/loc.Filesystem.ixx
      src/loc.Filesystem/DirectoryScanner.ixx
      src/loc.Filesystem/ExpandGlob.ixx
      src/loc.Filesystem/FileReader.ixx
)

# Counter module target
add_library(loc.Counter STATIC)
target_sources(loc.Counter
  PUBLIC
    FILE_SET CXX_MODULES FILES
      src/loc.Counter/loc.Counter.ixx
      src/loc.Counter/CLineCounter.ixx
      src/loc.Counter/PyLineCounter.ixx
      src/loc.Counter/FSLineCounter.ixx
      src/loc.Counter/XmlLineCounter.ixx
      src/loc.Counter/LineCounter.ixx
      src/loc.Counter/Counter.ixx
)
target_link_libraries(loc.Counter PRIVATE loc.Filesystem)

# Main executable
add_executable(loc
  "src/main.cpp"
)

target_link_libraries(loc PRIVATE loc.Filesystem loc.Counter CLI11::CLI11)

# Release flags
if (MSVC)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
else()
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()
