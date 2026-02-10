module;

#include <string>
#include <vector>
#include <filesystem>

export module loc.Counter:LineCounter;

import loc.Filesystem;

export class LineCounter
{
public:

    unsigned long CountLines(const std::filesystem::path& path, const std::string& inlineComment,
        const std::string& startMultilineComment, const std::string& endMultilineComment)
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
                    if (!InMultiLineComment && !startMultilineComment.empty())
                    {
                        auto sLen = startMultilineComment.size();
                        if (i + sLen <= line.size() && line.substr(i, sLen) == startMultilineComment)
                        {
                            InMultiLineComment = true;
                            i += static_cast<int>(sLen) - 1; // Skip over the start marker
                            continue;
                        }
                    }

                    if (InMultiLineComment && !endMultilineComment.empty())
                    {
                        auto eLen = endMultilineComment.size();
                        if (i + eLen <= line.size() && line.substr(i, eLen) == endMultilineComment)
                        {
                            InMultiLineComment = false;
                            i += static_cast<int>(eLen) - 1; // Skip over the end marker
                            if (!startMultilineComment.empty() && line.ends_with(endMultilineComment) && StrContains(line, startMultilineComment) && !line.starts_with(startMultilineComment))
                            {
                                countLine = true;
                            }
                            else if (!endMultilineComment.empty() && StrContains(line, endMultilineComment) && !line.ends_with(endMultilineComment))
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
            }

            // check for 1 line comments
            if (!inlineComment.empty() && line.starts_with(inlineComment))
                continue;

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