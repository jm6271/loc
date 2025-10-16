#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <filesystem>
#include <string>

import loc.Counter;

TEST_CASE("Count lines in a F# file")
{
	// Test files directory
	auto test_dir = std::string(TEST_DATA_DIR);

	std::string fs_path = test_dir + "/fs_file.fs";

	Counter counter(1, std::vector<std::filesystem::path>{ fs_path });

	REQUIRE(counter.Count() == 4);
}
