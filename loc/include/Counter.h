#pragma once

#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <filesystem>
#include <fstream>

#include "ILineCounter.h"

class Counter
{
public:
    Counter(unsigned int jobs, const std::vector<std::filesystem::path>& paths);

    Counter(unsigned int jobs, const std::filesystem::path& directoryPath, const std::vector<std::string>& extensions,
            bool includeGenerated, const std::vector<std::filesystem::path>& ignoreDirs);

    unsigned long Count();

private:
    unsigned int jobs{};
    std::vector<std::filesystem::path> paths{};

    bool IsDirectory(const std::filesystem::path& path) const;

    unsigned long CountFile(const std::filesystem::path& path) const;

    enum class FILE_LANGUAGE
    {
        C,
        Python,
        FSharp
    };

    FILE_LANGUAGE GetFileLanguage(const std::filesystem::path& path) const;

    bool isFileInDirectory(const std::filesystem::path& parentDir, const std::filesystem::path& filePath) const;

    void expandAllGlobsInPaths(const std::vector<std::filesystem::path>& paths_to_count);

    // multi-threading stuff
    
    std::atomic<size_t> next_index = 0;

    void CounterWorker();

    void GetNextPaths(std::vector<std::filesystem::path>& out_paths, int max_paths);

    std::atomic<unsigned long> total_lines{};
};
