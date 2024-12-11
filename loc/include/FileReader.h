#pragma once

#include <string>
#include <vector>


class FileReader
{
public:
	static void ReadFile(std::string path, std::vector<std::string>& output);

private:
	static std::string ltrim(const std::string& str);

	static std::string rtrim(const std::string& str);
};
