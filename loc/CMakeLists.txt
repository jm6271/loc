cmake_minimum_required(VERSION 3.14)
project(loc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.28)
  cmake_policy(SET CMP0144 NEW)
  cmake_policy(SET CMP0167 NEW)
endif()

find_package(CLI11 CONFIG REQUIRED)

set(LOC_SOURCES
    src/loc.Filesystem/DirectoryScanner.cpp
    src/loc.Filesystem/ExpandGlob.cpp
    src/loc.Filesystem/FileReader.cpp
    src/loc.Counter/Counter.cpp
    src/loc.Counter/LineCounter.cpp
)

add_executable(loc src/main.cpp ${LOC_SOURCES})
target_include_directories(loc PRIVATE src)
target_link_libraries(loc PRIVATE CLI11::CLI11)

# Release flags
if (MSVC)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
else()
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()
