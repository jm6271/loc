#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <vector>
#include <string>

import loc.Counter;

TEST_CASE("Count Lines in a XML file")
{
    // Test files directory
    auto test_dir = std::string(TEST_DATA_DIR);

    std::string file_path = test_dir + "/xml_file.xml";

    std::vector<std::filesystem::path> files = { file_path };

    Counter counter(1, files);

    REQUIRE(counter.Count() == 3);
}
