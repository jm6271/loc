# CMakeList.txt : CMake project for loc, include source and define
# project specific logic here.


if (WIN32) # WIN32 is true on a windows system
    set(BOOST_ROOT "C:/libs/Boost")
endif (WIN32)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_VERSION VERSION_GREATER 3.30)
    cmake_policy(SET CMP0144 NEW)
    cmake_policy(SET CMP0167 NEW)
endif()

find_package(Boost REQUIRED COMPONENTS program_options)

# Add source to this project's executable.
add_executable (loc "loc.cpp" "include/loc.h" 
    "include/ILineCounter.h" "include/CLineCounter.h" 
    "CLineCounter.cpp" "include/FileReader.h" "FileReader.cpp" 
    "include/DirectoryScanner.h" "DirectoryScanner.cpp" "include/Counter.h" 
    "Counter.cpp" "PyLineCounter.cpp" "include/PyLineCounter.h")

target_include_directories(loc PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(loc Boost::program_options)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET loc PROPERTY CXX_STANDARD 20)
endif()

# Add optimization flags for the Release build
if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()

