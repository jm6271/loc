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
std::vector<std::string> DirectoryScanner::Scan(const fs::path& directory, const std::vector<std::string>& extensions,
                                                const std::vector<std::string>& ignoreDirs) const
{
    std::vector<std::string> result;

    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        return result;
    }

    // Normalize ignore directories into absolute canonical paths
    std::vector<fs::path> ignorePaths;
    for (const auto& dir : ignoreDirs) {
        fs::path p(dir);
        if (p.is_absolute()) {
            ignorePaths.push_back(fs::weakly_canonical(p));
        } else {
            ignorePaths.push_back(fs::weakly_canonical(directory / p));
        }
    }

    fs::recursive_directory_iterator it(directory), end;
    while (it != end) {
        const fs::directory_entry& entry = *it;

        if (entry.is_directory()) {
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
                result.push_back(entry.path().string());
            }
        }

        ++it;
    }

    return result;
}

std::vector<std::string> DirectoryScanner::FindIgnoredDirectories(const std::filesystem::path &directory, const std::vector<std::string> &ignorePatterns) const
{
    std::vector<std::string> ignoredDirectories;

    if (fs::exists(directory) && fs::is_directory(directory))
    {
        for (const auto& entry : fs::recursive_directory_iterator(directory))
        {
            if (fs::is_directory(entry.status()))
            {
                // Check if the directory name matches any of the ignore patterns
                for (const auto& pattern : ignorePatterns)
                {
                    if (entry.path().filename().string() == pattern)
                    {
                        try
                        {
                            ignoredDirectories.push_back(entry.path().string());
                        }
                        catch(const std::system_error& e)
                        {
                            // Ignore the directory and print a warning
                            std::cerr << "Warning: System error thrown while getting a directory path. Skipping directory.\n";
                            std::cerr << "Error message: " << e.what() << '\n';
                            continue;
                        }
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
