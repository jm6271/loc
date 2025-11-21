module;

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <unordered_set>

export module loc.Filesystem:DirectoryScanner;

export class DirectoryScanner
{
public:
    DirectoryScanner() = default;

    std::vector<std::filesystem::path> Scan(
        const std::filesystem::path& directory,
        const std::vector<std::string>& extensions,
        const std::vector<std::filesystem::path>& ignoreDirs) const
    {
        std::vector<std::filesystem::path> result;
        if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
            return result;
        }

        // Pre-compute canonical ignore paths once
        std::vector<std::filesystem::path> ignorePaths;
        ignorePaths.reserve(ignoreDirs.size());
        for (const auto& dir : ignoreDirs) {
            try {
                auto canonical = dir.is_absolute()
                    ? std::filesystem::weakly_canonical(dir)
                    : std::filesystem::weakly_canonical(directory / dir);
                ignorePaths.push_back(std::move(canonical));
            }
            catch (...) {
                // Skip paths that can't be canonicalized
            }
        }

        // Use unordered_set for O(1) extension lookups
        std::unordered_set<std::string> extSet(extensions.begin(), extensions.end());

        // Use error code version to avoid exceptions
        std::error_code ec;
        std::filesystem::recursive_directory_iterator it(directory, ec);
        std::filesystem::recursive_directory_iterator end;

        while (it != end) {
            const auto& entry = *it;

            if (entry.is_directory(ec)) {
                // Only canonicalize if we have ignore paths
                if (!ignorePaths.empty()) {
                    auto current = std::filesystem::weakly_canonical(entry.path(), ec);
                    if (!ec && shouldIgnore(current, ignorePaths)) {
                        it.disable_recursion_pending();
                    }
                }
            }
            else if (entry.is_regular_file(ec)) {
                // Avoid string allocation - compare string_view if possible
                const auto& ext = entry.path().extension();
                if (!ext.empty() && extSet.count(ext.string()) > 0) {
                    result.push_back(entry.path());
                }
            }

            it.increment(ec);
            if (ec) {
                // Log error if needed, then continue
                ec.clear();
            }
        }

        return result;
    }

    std::vector<std::filesystem::path> FindIgnoredDirectories(
        const std::filesystem::path& directory,
        const std::vector<std::string>& ignorePatterns) const
    {
        std::vector<std::filesystem::path> ignoredDirectories;

        if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
            std::cerr << "Directory does not exist or is not a directory '"
                << directory << '\'' << std::endl;
            return ignoredDirectories;
        }

        // Use unordered_set for faster pattern matching
        std::unordered_set<std::string> patternSet(ignorePatterns.begin(), ignorePatterns.end());

        std::error_code ec;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory, ec))
        {
            if (entry.is_directory(ec))
            {
                auto filename = entry.path().filename().string();
                if (patternSet.count(filename) > 0)
                {
                    ignoredDirectories.push_back(entry.path());
                }
            }
        }

        return ignoredDirectories;
    }

private:
    bool shouldIgnore(const std::filesystem::path& current,
        const std::vector<std::filesystem::path>& ignorePaths) const
    {
        return std::any_of(ignorePaths.begin(), ignorePaths.end(),
            [&](const std::filesystem::path& ignore) {
                return isSubPath(ignore, current);
            });
    }

    bool isSubPath(const std::filesystem::path& base, const std::filesystem::path& path) const
    {
        // Paths are already canonical, no need to canonicalize again
        auto baseIt = base.begin();
        auto pathIt = path.begin();

        while (baseIt != base.end() && pathIt != path.end()) {
            if (*baseIt != *pathIt) {
                return false;
            }
            ++baseIt;
            ++pathIt;
        }

        return baseIt == base.end();
    }
};
