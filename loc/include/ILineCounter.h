// Basic interface for a line counting class

#pragma once

#include <vector>
#include <filesystem>


class ILineCounter
{
public:
	/// <summary>
	/// Count the lines of code in the given file
	/// </summary>
	/// <returns>The number of lines </returns>
	virtual unsigned long CountLines(const std::filesystem::path& path) = 0;

	virtual ~ILineCounter() = default;
};
