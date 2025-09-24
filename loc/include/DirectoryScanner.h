#pragma once

#include <string>
#include <vector>
#include <filesystem>

// Class for scanning files in a directory
class DirectoryScanner
{
public:
    DirectoryScanner() = default;

    std::vector<std::filesystem::path> Scan(const std::filesystem::path& directory, const std::vector<std::string>& extensions,
        const std::vector<std::filesystem::path>& ignoreDirs) const;

    std::vector<std::filesystem::path> FindIgnoredDirectories(const std::filesystem::path& directory, const std::vector<std::string>& ignorePatterns) const;

private:
    bool isSubPath(const std::filesystem::path& base, const std::filesystem::path& path) const;
};
