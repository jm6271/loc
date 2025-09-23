#pragma once

#include <string>
#include <vector>
#include <filesystem>

// Class for scanning files in a directory
class DirectoryScanner
{
public:
    DirectoryScanner() = default;

    std::vector<std::string> Scan(const std::filesystem::path& directory, const std::vector<std::string>& extensions,
        const std::vector<std::string>& ignoreDirs) const;

    std::vector<std::string> FindIgnoredDirectories(const std::filesystem::path& directory, const std::vector<std::string>& ignorePatterns) const;

private:
    bool isSubPath(const std::filesystem::path& base, const std::filesystem::path& path) const;
};
