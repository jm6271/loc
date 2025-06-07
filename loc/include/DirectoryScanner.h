#pragma once

#include <string>
#include <vector>
#include <filesystem>

// Class for scanning files in a directory
class DirectoryScanner
{
public:
    DirectoryScanner() = default;

    std::vector<std::string> Scan(const std::filesystem::path& directory, const std::vector<std::string>& extensions) const;
};
