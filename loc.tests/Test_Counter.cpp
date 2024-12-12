#include <catch2/catch_test_macros.hpp>
#include "include/Counter.h"

TEST_CASE("Test Counter with ignores")
{
    // Path to test files directory
    auto test_dir = std::string(TEST_DATA_DIR);

    Counter counter(4, ".cpp;.h;.py", {test_dir}, test_dir + "/header.h");

    auto result = counter.Count();

    REQUIRE(result == 11);

}

TEST_CASE("Test Counter without ignores")
{
        // Path to test files directory
    auto test_dir = std::string(TEST_DATA_DIR);

    Counter counter(4, ".cpp;.h;.py", {test_dir}, "");

    auto result = counter.Count();

    REQUIRE(result == 24);
}

TEST_CASE("Test Counter with file paths")
{
        // Path to test files directory
    auto test_dir = std::string(TEST_DATA_DIR);

    Counter counter(4, ".cpp;.h;.py", {test_dir + "/py_file.py", test_dir + "/cpp_file.cpp"}, "");

    auto result = counter.Count();

    REQUIRE(result == 11);
}
