#include "include/DirectoryScanner.h"
#include <iostream>
#include <algorithm>

namespace fs = std::filesystem;

/**
* Scans a directory and returns a list of all the paths to the files with the given extensions
* @param directory the directory to scan
* @param extensions the extensions to look for
* @return a list of paths to the files found
*/
std::vector<fs::path> DirectoryScanner::Scan(const fs::path& directory, const std::vector<std::string>& extensions,
                                                const std::vector<fs::path>& ignoreDirs) const
{
    std::vector<fs::path> result;

    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        return result;
    }

    // Normalize ignore directories into absolute canonical paths
    std::vector<fs::path> ignorePaths;
    if (!ignoreDirs.empty())
    {
        for (const auto& dir : ignoreDirs) {
            if (dir.is_absolute()) {
                ignorePaths.push_back(fs::weakly_canonical(dir));
            } else {
                ignorePaths.push_back(fs::weakly_canonical(directory / dir));
            }
        }
    }

    fs::recursive_directory_iterator it(directory);
    fs::recursive_directory_iterator end;
    while (it != end) {

        if (const fs::directory_entry& entry = *it; entry.is_directory() && ignorePaths.empty()) {
            fs::path current = fs::weakly_canonical(entry.path());

            // Skip if inside any ignored directory
            bool skip = std::any_of(ignorePaths.begin(), ignorePaths.end(),
                                    [&](const fs::path& ignore) {
                                        return isSubPath(ignore, current);
                                    });

            if (skip) {
                it.disable_recursion_pending(); // skip this whole subtree
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

std::vector<fs::path> DirectoryScanner::FindIgnoredDirectories(const std::filesystem::path &directory, const std::vector<std::string> &ignorePatterns) const
{
    std::vector<fs::path> ignoredDirectories;

    if (fs::exists(directory) && fs::is_directory(directory))
    {
        for (const auto& entry : fs::recursive_directory_iterator(directory))
        {
            if (fs::is_directory(entry.status()))
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

bool DirectoryScanner::isSubPath(const std::filesystem::path &base, const std::filesystem::path &path) const
{
    auto baseAbs = fs::weakly_canonical(base);
    auto pathAbs = fs::weakly_canonical(path);

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
