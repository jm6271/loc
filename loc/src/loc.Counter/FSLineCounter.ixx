module;

#include <filesystem>

export module loc.Counter:FSLineCounter;

import loc.Filesystem;

export class FSLineCounter
{
public:
	unsigned long CountLines(const std::filesystem::path& path)
	{
		unsigned long totalLines{};

		// Read file and get a vector of lines
		std::vector<std::string> lines{};
		FileReader::ReadFile(path, lines);

		bool InMultiLineComment{ false };

		// count the lines
		for (const auto& line : lines)
		{
			// check for whitespace
			if (line == "")
				continue;

			// check for 1 line comments
			if (line.starts_with("//"))
				continue;

			// check for multiline comments
			if (line.starts_with("(*"))
			{
				if (line.ends_with("*)"))
					continue;

				InMultiLineComment = true;
				continue;
			}

			if (InMultiLineComment)
			{
				if (line.ends_with("*)"))
					InMultiLineComment = false;
				continue;
			}

			// if we made it to here, this is valid code
			totalLines++;
		}

		return totalLines;
	}
};
