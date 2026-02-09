module;

#include <filesystem>
#include <vector>
#include <string>

export module loc.Counter:FSLineCounter;

import loc.Filesystem;
import :LineCounter;

export class FSLineCounter
{
public:
    unsigned long CountLines(const std::filesystem::path& path)
    {
        LineCounter counter;
        return counter.CountLines(path, "//", "(*", "*)");
    }

};
