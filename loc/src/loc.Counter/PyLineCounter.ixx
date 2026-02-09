module;

#include <filesystem>
#include <string>
#include <vector>

export module loc.Counter:PyLineCounter;

import loc.Filesystem;
import :LineCounter;

export class PyLineCounter
{
public:
    unsigned long CountLines(const std::filesystem::path& path)
    {
        LineCounter counter;
        return counter.CountLines(path, "#", "", "");
    }
};
