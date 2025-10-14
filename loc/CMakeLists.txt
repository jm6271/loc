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
add_library(Filesystem STATIC)
target_sources(Filesystem
  PUBLIC
    FILE_SET CXX_MODULES FILES
      src/Filesystem/Filesystem.ixx
      src/Filesystem/Filesystem_DirectoryScanner.ixx
)

# Main executable
add_executable(loc
  loc.cpp
  include/loc.h
  include/ILineCounter.h
  include/CLineCounter.h
  CLineCounter.cpp
  include/FileReader.h
  FileReader.cpp
  include/Counter.h
  Counter.cpp
  PyLineCounter.cpp
  include/PyLineCounter.h
  include/FSLineCounter.h
  FSLineCounter.cpp
  include/ExpandGlob.h
  ExpandGlob.cpp
)

target_link_libraries(loc PRIVATE Filesystem CLI11::CLI11)

# Release flags
if (MSVC)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
else()
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()
