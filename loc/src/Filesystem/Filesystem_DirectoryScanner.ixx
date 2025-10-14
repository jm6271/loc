module;

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <algorithm>

export module Filesystem:DirectoryScanner;


// Class for scanning files in a directory
export class DirectoryScanner
{
public:
    DirectoryScanner() = default;

    std::vector<std::filesystem::path> Scan(const std::filesystem::path& directory, const std::vector<std::string>& extensions,
        const std::vector<std::filesystem::path>& ignoreDirs) const
    {
        std::vector<std::filesystem::path> result;
        if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
            return result;
        }

        // Normalize ignore directories into absolute canonical paths
        std::vector<std::filesystem::path> ignorePaths;
        if (!ignoreDirs.empty())
        {
            for (const auto& dir : ignoreDirs) {
                if (dir.is_absolute()) {
                    ignorePaths.push_back(std::filesystem::weakly_canonical(dir));
                }
                else {
                    ignorePaths.push_back(std::filesystem::weakly_canonical(directory / dir));
                }
            }
        }

        std::filesystem::recursive_directory_iterator it(directory);
        std::filesystem::recursive_directory_iterator end;

        while (it != end) {
            const std::filesystem::directory_entry& entry = *it;

            if (entry.is_directory() && !ignorePaths.empty()) {
                std::filesystem::path current = std::filesystem::weakly_canonical(entry.path());
                // Skip if inside any ignored directory
                bool skip = std::any_of(ignorePaths.begin(), ignorePaths.end(),
                    [&](const std::filesystem::path& ignore) {
                        return isSubPath(ignore, current);
                    });
                if (skip) {
                    it.disable_recursion_pending();
                }
            }
            else if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (std::find(extensions.begin(), extensions.end(), ext) != extensions.end()) {
                    result.push_back(entry.path());
                }
            }
            ++it;
        }
        return result;
    }

    std::vector<std::filesystem::path> FindIgnoredDirectories(const std::filesystem::path& directory, const std::vector<std::string>& ignorePatterns) const
    {
        std::vector<std::filesystem::path> ignoredDirectories;

        if (std::filesystem::exists(directory) && std::filesystem::is_directory(directory))
        {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
            {
                if (std::filesystem::is_directory(entry.status()))
                {
                    // Check if the directory name matches any of the ignore patterns
                    for (const auto& pattern : ignorePatterns)
                    {
                        if (entry.path().filename() == pattern)
                        {
                            ignoredDirectories.push_back(entry.path());
                            break; // No need to check other patterns for this directory
                        }
                    }
                }
            }
        }
        else
        {
            std::cerr << "Directory does not exist or is not a directory '" << directory << '\'' << std::endl;
        }

        return ignoredDirectories;
    }

private:

    bool isSubPath(const std::filesystem::path& base, const std::filesystem::path& path) const
    {
        auto baseAbs = std::filesystem::weakly_canonical(base);
        auto pathAbs = std::filesystem::weakly_canonical(path);

        auto baseIt = baseAbs.begin();
        auto pathIt = pathAbs.begin();

        for (; baseIt != baseAbs.end() && pathIt != pathAbs.end(); ++baseIt, ++pathIt) {
            if (*baseIt != *pathIt) {
                return false;
            }
        }

        // True if base was fully consumed (base == path, or base is a prefix of path)
        return baseIt == baseAbs.end();
    }
};
