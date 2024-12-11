#include "include/FileReader.h"
#include <fstream>
#include <iostream>

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
			output.push_back(rtrim(ltrim(line)));
		}
		file.close();
	}
	else
	{
		std::cerr << "Error: unable to open file: " << path << "\n";
	}
}

/**
 * \brief Remove leading whitespace from a string.
 *
 * \param str The input string to trim.
 * \return A new string with leading whitespace removed.
 *
 * \details This function removes any leading whitespace characters from the
 * input string and returns the trimmed result. The original string is not modified.
 */
std::string FileReader::ltrim(const std::string& str)
{
	std::string s = str;
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		[](unsigned char ch) {
			return !std::isspace(ch);
		}));

	return s;
}

std::string FileReader::rtrim(const std::string &str)
{
	std::string s = str;
	s.erase(std::find_if(s.rbegin(), s.rend(),
		[](unsigned char ch) {
			return !std::isspace(ch);
		}).base(), s.end());
	return s;
}
