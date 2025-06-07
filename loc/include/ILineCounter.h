// Basic interface for a line counting class

#pragma once

#include <vector>
#include <string>


class ILineCounter
{
public:
	/// <summary>
	/// Count the lines of code in the given file
	/// </summary>
	/// <returns>The number of lines </returns>
	virtual unsigned long CountLines(const std::string& path) = 0;

	virtual ~ILineCounter() = default;
};
