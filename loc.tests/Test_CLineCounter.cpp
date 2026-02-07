#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <vector>
#include <string>

import loc.Counter;

TEST_CASE("Count Lines in a C++ file")
{
    // Test files directory
    auto test_dir = std::string(TEST_DATA_DIR);

    std::string cpp_path = test_dir + "/cpp_file.cpp";
    std::string header_path = test_dir + "/header.h";

	std::vector<std::filesystem::path> files = { cpp_path, header_path };

	Counter counter(1, files);

    REQUIRE(counter.Count() == 21);
}
