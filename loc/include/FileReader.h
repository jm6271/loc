#pragma once

#include <string>
#include <vector>


class FileReader
{
public:
	static void ReadFile(std::string path, std::vector<std::string>& output);
};
