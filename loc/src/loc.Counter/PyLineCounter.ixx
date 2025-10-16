module;

#include <filesystem>
#include <string>
#include <vector>

export module loc.Counter:PyLineCounter;

import loc.Filesystem;

export class PyLineCounter
{
public:
    unsigned long CountLines(const std::filesystem::path& path)
    {
		unsigned long totalLines{};

		// Read file and get a vector of lines
		std::vector<std::string> lines{};
		FileReader::ReadFile(path, lines);

		// count the lines
		for (const auto& line : lines)
		{
			// check for whitespace
			if (line == "")
				continue;

			// check for 1 line comments
			if (line.starts_with("#"))
				continue;

			// if we made it to here, this is valid code
			totalLines++;
		}

		return totalLines;
    }
};
