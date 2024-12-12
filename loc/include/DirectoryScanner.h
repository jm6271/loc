#pragma once

#include <string>
#include <vector>

// Class for scanning files in a directory
class DirectoryScanner
{
public:
    DirectoryScanner();

    std::vector<std::string> Scan(std::string directory, std::vector<std::string> extensions);
};
