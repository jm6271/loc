#include <catch2/catch_test_macros.hpp>
#include "include/PyLineCounter.h"

TEST_CASE("Count lines in a Python file")
{
    PyLineCounter counter;

    // Test files directory
    auto test_dir = std::string(TEST_DATA_DIR);

    REQUIRE(counter.CountLines(test_dir + "/py_file.py") == 5);
}
