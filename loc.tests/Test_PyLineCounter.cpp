#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <vector>
#include <string>

import loc.Counter;

TEST_CASE("Count lines in a Python file")
{
    // Test files directory
    auto test_dir = std::string(TEST_DATA_DIR);

    std::filesystem::path py_path = test_dir + "/py_file.py";

	Counter counter(1, std::vector{ py_path });

    REQUIRE(counter.Count() == 5);
}
