# CMakeList.txt : CMake project for loc.tests, include source and define
# project specific logic here.


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_VERSION VERSION_GREATER 3.28)
  cmake_policy(SET CMP0144 NEW)
  cmake_policy(SET CMP0167 NEW)
endif()


add_compile_definitions(TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test_files")

find_package(Catch2 3 REQUIRED)

set(LOC_SOURCES
    ../loc/src/DirectoryScanner.cpp
    ../loc/src/ExpandGlob.cpp
    ../loc/src/FileReader.cpp
    ../loc/src/Counter.cpp
    ../loc/src/LineCounter.cpp
)

# Add source to this project's executable.
add_executable(loc.tests
    main.cpp
    Test_Counter.cpp
    Test_CLineCounter.cpp
    Test_DirectoryScanner.cpp
    Test_ExpandGlob.cpp
    Test_FSLineCounter.cpp
    Test_PyLineCounter.cpp
    Test_XmlLineCounter.cpp
    ${LOC_SOURCES}
)

target_include_directories(loc.tests PRIVATE ${Catch2_INCLUDE_DIRS} ../loc/include)
target_link_libraries(loc.tests PRIVATE Catch2::Catch2WithMain)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET loc.tests PROPERTY CXX_STANDARD 20)
endif()

include(CTest)
include(Catch)
catch_discover_tests(loc.tests)
