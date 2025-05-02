#include <catch2/catch_test_macros.hpp>
#include "include/ExpandGlob.h"

TEST_CASE("Glob expands correctly")
{
    // Test files directory
    auto test_dir = std::string(TEST_DATA_DIR);

    // Expected data
    std::vector<std::string> expected{};

    expected.push_back(test_dir + "/cpp_file2.cpp");
    expected.push_back(test_dir + "/cpp_file.cpp");

    // Actual data
    std::vector<std::string> actual{};

    // Act
    ExpandGlob expander{};
    expander.expand_glob(test_dir + "/*.cpp", actual);

    // Sort the vectors to make sure the items are in the same order
    std::sort(actual.begin(), actual.end());
    std::sort(expected.begin(), expected.end());

    // Normalize path separater character to '/'
    for (auto& path : actual)
    {
        std::replace(path.begin(), path.end(), '\\', '/');
    }

    for (auto& path : expected)
    {
        std::replace(path.begin(), path.end(), '\\', '/');
    }

    // Assert
    REQUIRE(actual == expected);
}
