#include "include/FileReader.h"
#include <fstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

/**
 * \brief Read a file line-by-line and store the lines in the output vector
 *
 * \param path The path to the file to read
 * \param output The vector to store the lines in
 *
 * \details This function reads a file line-by-line and appends each line to the
 * given output vector. The function trims any leading whitespace from each line
 * before adding it to the vector. If the file cannot be opened, an error message
 * is printed to the standard error stream.
 */
void FileReader::ReadFile(std::string path, std::vector<std::string>& output)
{
	std::ifstream file{ path };
	std::string line{};

	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			boost::trim(line);
			output.push_back(line);
		}
		file.close();
	}
	else
	{
		std::cerr << "Error: unable to open file: " << path << "\n";
	}
}
