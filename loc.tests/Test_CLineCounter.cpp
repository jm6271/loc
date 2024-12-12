#include <catch2/catch_test_macros.hpp>

#include "include/CLineCounter.h"

TEST_CASE("CountLines")
{
    CLineCounter counter;

    // Test files directory
    auto test_dir = std::string(TEST_DATA_DIR);

    REQUIRE(counter.CountLines(test_dir + "/cpp_file.cpp") == 6);
    REQUIRE(counter.CountLines(test_dir + "/header.h") == 13);
}
