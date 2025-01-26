# CMakeList.txt : CMake project for loc.tests, include source and define
# project specific logic here.


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_VERSION VERSION_GREATER 3.30)
  cmake_policy(SET CMP0144 NEW)
  cmake_policy(SET CMP0167 NEW)
endif()


add_compile_definitions(TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test_files")

find_package(Catch2 3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS algorithm)

# Add source to this project's executable.
add_executable (loc.tests "main.cpp" "Test_CLineCounter.cpp" "Test_PyLineCounter.cpp" "Test_DirectoryScanner.cpp" "Test_Counter.cpp"
    "../loc/CLineCounter.cpp" "../loc/FileReader.cpp" "../loc/PyLineCounter.cpp" "../loc/DirectoryScanner.cpp" "../loc/Counter.cpp")

target_include_directories(loc.tests PRIVATE ${Catch2_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
target_link_libraries(loc.tests PRIVATE Catch2::Catch2WithMain Boost::algorithm)

include_directories("../loc")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET loc.tests PROPERTY CXX_STANDARD 20)
endif()

include(Catch)
catch_discover_tests(loc.tests)
