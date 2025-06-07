#pragma once

#include <string>
#include <vector>


class FileReader
{
public:
	static void ReadFile(const std::string& path, std::vector<std::string>& output);
private:
	static void ltrim(std::string& s);
	static void rtrim(std::string& s);
	static void trim(std::string& s);
};
