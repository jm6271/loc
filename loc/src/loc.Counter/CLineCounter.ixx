module;

#include <filesystem>
#include <vector>
#include <string>

export module loc.Counter:CLineCounter;

import loc.Filesystem;
import :LineCounter;

export class CLineCounter
{
public:

    unsigned long CountLines(const std::filesystem::path& path)
    {
        LineCounter counter;
        return counter.CountLines(path, "//", "/*", "*/");
    }

};
