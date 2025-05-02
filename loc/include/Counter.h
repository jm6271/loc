#pragma once

#include <vector>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <filesystem>

#include "ILineCounter.h"

class Counter
{
public:
    Counter(unsigned int jobs, const std::vector<std::string>& paths);

    unsigned long Count();

private:
    unsigned int jobs{};
    std::vector<std::string> paths{};

    bool IsDirectory(const std::filesystem::path& path) const;

    unsigned long CountFile(std::string path);

    void normalizePath(std::string& path) const;

    enum class FILE_LANGUAGE
    {
        C,
        Python,
        FSharp
    };

    FILE_LANGUAGE GetFileLanguage(const std::string& path) const;

    bool isFileInDirectory(const std::filesystem::path& parentDir, const std::filesystem::path& filePath) const;

    void expandAllGlobsInPaths(std::vector<std::string>& paths);


    // multi-threading stuff
    std::queue<std::string> file_queue{};

    std::mutex file_queue_mutex{};

    void CounterWorker();

    std::atomic<unsigned long> total_lines{};
};
