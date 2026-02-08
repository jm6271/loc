module;

#include <string>
#include <vector>
#include <filesystem>

export module loc.Counter:XmlLineCounter;

import loc.Filesystem;

export class XmlLineCounter
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

            bool InString{ false };
            bool countLine{ true };

            // check for multiline comments
            for (size_t i = 0; i < line.size(); ++i)
            {
                char c = line[i];

                // Toggle string state on encountering a double quote, ignoring escaped quotes
                if (c == '\"' && (i == 0 || line[i - 1] != '\\'))
                {
                    InString = !InString;
                }

                if (!InString)
                {
                    if (!InMultiLineComment && i + 3 < line.size() && line[i] == '<' && line[i + 1] == '!'
                        && line[i + 2] == '-' && line[i + 3] == '-')
                    {
                        InMultiLineComment = true;
                        i = i + 3; // skip the '!--' characters
                        continue;
                    }

                    if (InMultiLineComment && i + 2 < line.size() && line[i] == '-' && line[i + 1] == '-' && line[i + 2] == '>')
                    {
                        InMultiLineComment = false;
                        i += 2; // Skip the '->' characters
                        if (line.ends_with("-->") && StrContains(line, "<!--") && !line.starts_with("<!--"))
                        {
                            countLine = true;
                        }
                        else if (StrContains(line, "-->") && !line.ends_with("-->"))
                        {
                            countLine = true;
                        }
                        else
                        {
                            // full line was commented
                            countLine = false;
                        }
                        continue;
                    }
                }
            }

            if (!InMultiLineComment && !InString && countLine)
            {
                totalLines++;
            }
        }

        return totalLines;
    }
private:
	bool StrContains(const std::string& str, const std::string& substr)
	{
		return str.find(substr) != std::string::npos;
	}
};
