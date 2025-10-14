#include <catch2/catch_test_macros.hpp>
#include <algorithm>
#include <filesystem>

import Filesystem;

TEST_CASE("Test DirectoryScanner")
{
    DirectoryScanner scanner;

    // Test files directory
    auto test_dir = std::string(TEST_DATA_DIR);

    // Expected data
    std::vector<std::filesystem::path> expected{};

    expected.push_back(test_dir + "/py_file.py");
    expected.push_back(test_dir + "/cpp_file.cpp");
    expected.push_back(test_dir + "/cpp_file2.cpp");
    expected.push_back(test_dir + "/header.h");

    // Actual data
    std::vector<std::filesystem::path> actual{};
    actual = scanner.Scan(test_dir, {".cpp", ".py", ".h"}, {});

    // Check if the actual data matches the expected data
    // Sort the vectors first to make sure items are in the same order
    std::sort(actual.begin(), actual.end());
    std::sort(expected.begin(), expected.end());

    // Normalize path separater character to '/'
    for (auto& path : actual)
    {
        //std::replace(path.begin(), path.end(), '\\', '/');
        path.make_preferred();
    }

    for (auto& path : expected)
    {
        //std::replace(path.begin(), path.end(), '\\', '/');
		path.make_preferred();
    }

    REQUIRE(actual == expected);
}
