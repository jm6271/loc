# CMakeList.txt : CMake project for loc.tests, include source and define
# project specific logic here.


if (WIN32) # WIN32 is true on a windows system
    set(CATCH2_ROOT "C:/libs/Catch2")
endif (WIN32)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_policy(SET CMP0144 NEW)
cmake_policy(SET CMP0167 NEW)

add_compile_definitions(TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test_files")

find_package(Catch2 3 REQUIRED)

# Add source to this project's executable.
add_executable (loc.tests "main.cpp" "Test_CLineCounter.cpp" "Test_PyLineCounter.cpp" "Test_DirectoryScanner.cpp"
    "../loc/CLineCounter.cpp" "../loc/FileReader.cpp" "../loc/PyLineCounter.cpp" "../loc/DirectoryScanner.cpp")

target_include_directories(loc.tests PRIVATE ${Catch2_INCLUDE_DIRS})
target_link_libraries(loc.tests PRIVATE Catch2::Catch2WithMain)

include_directories("../loc")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET loc.tests PROPERTY CXX_STANDARD 20)
endif()
