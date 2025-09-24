#pragma once

#include <string>
#include <vector>
#include <filesystem>


class FileReader
{
public:
	static void ReadFile(const std::filesystem::path& path, std::vector<std::string>& output);
private:
	static void ltrim(std::string& s);
	static void rtrim(std::string& s);
	static void trim(std::string& s);
};
