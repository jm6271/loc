#include <catch2/catch_test_macros.hpp>

#include "include/CLineCounter.h"

TEST_CASE("Count Lines in a C++ file")
{
    CLineCounter counter;

    // Test files directory
    auto test_dir = std::string(TEST_DATA_DIR);

    std::string cpp_path = test_dir + "/cpp_file.cpp";
    std::string header_path = test_dir + "/header.h";

    REQUIRE(counter.CountLines(cpp_path) == 6);
    REQUIRE(counter.CountLines(header_path) == 13);
}
