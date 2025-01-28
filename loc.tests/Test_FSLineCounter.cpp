#include <catch2/catch_test_macros.hpp>

#include "include/FSLineCounter.h"

TEST_CASE("Count lines in a F# file")
{
	FSLineCounter counter;

	// Test files directory
	auto test_dir = std::string(TEST_DATA_DIR);

	std::string fs_path = test_dir + "/fs_file.fs";

	REQUIRE(counter.CountLines(fs_path) == 4);
}
