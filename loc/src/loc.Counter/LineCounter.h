#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include "loc.Filesystem/FileReader.h"

class LineCounter
{
public:

    unsigned long CountLines(const std::filesystem::path& path, const std::string& inlineComment,
        const std::string& startMultilineComment, const std::string& endMultilineComment);

private:

    bool StrContains(const std::string& str, const std::string& substr);
};
