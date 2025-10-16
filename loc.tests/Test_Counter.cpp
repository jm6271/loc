#include <catch2/catch_test_macros.hpp>

import loc.Counter;

TEST_CASE("Test Counter with glob")
{
        // Path to test files directory
    auto test_dir = std::string(TEST_DATA_DIR);

    Counter counter(4, { test_dir + "/*.*" });

    auto result = counter.Count();

    REQUIRE(result == 28);
}

TEST_CASE("Test Counter without glob")
{
        // Path to test files directory
    auto test_dir = std::string(TEST_DATA_DIR);

    Counter counter(4, {test_dir + "/py_file.py", test_dir + "/cpp_file.cpp"});

    auto result = counter.Count();

    REQUIRE(result == 11);
}

TEST_CASE("Test Counter with both globs and full paths")
{
    // Path to test files directory
    auto test_dir = std::string(TEST_DATA_DIR);
    Counter counter(4, { test_dir + "/py_file.py", test_dir + "/*.cpp" });
    auto result = counter.Count();
    REQUIRE(result == 11);
}
