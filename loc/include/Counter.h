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
    Counter(unsigned int jobs, std::string extensions, std::vector<std::string> paths, std::string ignores);

    unsigned long Count();

private:
    unsigned int jobs{};
    std::vector<std::string> extensions{};
    std::vector<std::string> paths{};
    std::vector<std::string> ignorePaths{};

    bool IsDirectory(std::string path);

    std::vector<std::string> GetExtensions(std::string extensions);

    std::vector<std::string> GetIgnorePaths(std::string ignores);

    bool IsIgnored(std::string path);

    unsigned long CountFile(std::string path);

    void normalizePath(std::string& path);

    enum class FILE_LANGUAGE
    {
        C,
        Python
    };

    FILE_LANGUAGE GetFileLanguage(std::string path);

    bool isFileInDirectory(const std::filesystem::path& parentDir, const std::filesystem::path& filePath);

    std::vector<std::string> GetAllFilesWithExtensionsInDirectories();


    // multi-threading stuff
    std::queue<std::string> file_queue{};

    std::mutex file_queue_mutex{};

    void CounterWorker();

    std::atomic<unsigned long> total_lines{};
};
