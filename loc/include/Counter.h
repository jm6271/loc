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
    Counter(unsigned int jobs, const std::string& extensions, const std::vector<std::string>& paths, const std::string& ignores);

    unsigned long Count();

private:
    unsigned int jobs{};
    std::vector<std::string> extensions{};
    std::vector<std::string> paths{};
    std::vector<std::string> ignorePaths{};

    bool IsDirectory(const std::filesystem::path& path) const;

    std::vector<std::string> GetExtensions(const std::string& extensions_string) const;

    std::vector<std::string> GetIgnorePaths(const std::string& ignores) const;

    bool IsIgnored(std::string path);

    unsigned long CountFile(std::string path);

    void normalizePath(std::string& path) const;

    enum class FILE_LANGUAGE
    {
        C,
        Python
    };

    FILE_LANGUAGE GetFileLanguage(const std::string& path) const;

    bool isFileInDirectory(const std::filesystem::path& parentDir, const std::filesystem::path& filePath) const;

    std::vector<std::string> GetAllFilesWithExtensionsInDirectories();


    // multi-threading stuff
    std::queue<std::string> file_queue{};

    std::mutex file_queue_mutex{};

    void CounterWorker();

    std::atomic<unsigned long> total_lines{};
};
