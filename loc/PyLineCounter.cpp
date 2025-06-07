#include "include/PyLineCounter.h"
#include "include/FileReader.h"

/**
 * Count the number of lines of code in a Python file.
 * 
 * @param path The path to the file to be counted.
 * @return The number of lines of code in the given file.
 * 
 * This function reads a file line-by-line and counts the number of lines of code.
 * It skips whitespace lines and single-line comments that start with '#'.
 */
unsigned long PyLineCounter::CountLines(const std::string& path)
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
