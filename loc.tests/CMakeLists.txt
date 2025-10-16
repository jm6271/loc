# CMakeList.txt : CMake project for loc.tests, include source and define
# project specific logic here.


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_VERSION VERSION_GREATER 3.28)
  cmake_policy(SET CMP0144 NEW)
  cmake_policy(SET CMP0167 NEW)
endif()


add_compile_definitions(TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test_files")

find_package(Catch2 3 REQUIRED)

# Add source to this project's executable.
add_executable (loc.tests "main.cpp" "Test_CLineCounter.cpp" "Test_PyLineCounter.cpp" "Test_DirectoryScanner.cpp" "Test_Counter.cpp"
                  "Test_FSLineCounter.cpp"  "Test_ExpandGlob.cpp" )

target_include_directories(loc.tests PRIVATE ${Catch2_INCLUDE_DIRS})
target_link_libraries(loc.tests PRIVATE Catch2::Catch2WithMain loc.Filesystem loc.Counter)

include_directories("../loc")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET loc.tests PROPERTY CXX_STANDARD 20)

  # Enable experimental modules support
  if(MSVC)
    target_compile_options(loc.tests PRIVATE /experimental:module)
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(loc.tests PRIVATE -fmodules-ts)
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(loc.tests PRIVATE -fmodules)
  endif()
endif()

include(Catch)
catch_discover_tests(loc.tests)
