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
    Counter(unsigned int jobs, const std::vector<std::string>& paths);

    Counter(unsigned int jobs, const std::string& directoryPath, const std::vector<std::string>& extensions,
            bool includeGenerated, const std::vector<std::string>& ignoreDirs);

    unsigned long Count();

private:
    unsigned int jobs{};
    std::vector<std::string> paths{};

    bool IsDirectory(const std::filesystem::path& path) const;

    unsigned long CountFile(const std::string& path) const;

    void normalizePath(std::string& path) const;

    enum class FILE_LANGUAGE
    {
        C,
        Python,
        FSharp
    };

    FILE_LANGUAGE GetFileLanguage(const std::string& path) const;

    bool isFileInDirectory(const std::filesystem::path& parentDir, const std::filesystem::path& filePath) const;

    void expandAllGlobsInPaths(const std::vector<std::string>& paths_to_count);

    // multi-threading stuff
    
    std::atomic<size_t> next_index = 0;

    void CounterWorker();

    void GetNextPaths(std::vector<std::string>& out_paths, int max_paths);

    std::atomic<unsigned long> total_lines{};
};
