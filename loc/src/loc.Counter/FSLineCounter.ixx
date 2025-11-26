module;

#include <filesystem>
#include <vector>
#include <string>

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
			if (StrContains(line, "(*"))
			{
				if (StrContains(line, "*)"))
				{
					InMultiLineComment = false;
					if (!line.starts_with("(*") || !line.ends_with("*)"))
					{
						totalLines++;
					}
					continue;
				}

				InMultiLineComment = true;
				continue;
			}

			if (StrContains(line, "*)"))
			{
				InMultiLineComment = false;
				continue;
			}

			if (InMultiLineComment)
			{
				continue;
			}

			// if we made it to here, this is valid code
			totalLines++;
		}

		return totalLines;
	}

private:

	bool StrContains(const std::string& str, const std::string& substr)
	{
		return str.find(substr) != std::string::npos;
	}
};
