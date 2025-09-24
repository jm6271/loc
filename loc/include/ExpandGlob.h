#pragma once

#include <filesystem>
#include <regex>
#include <string>
#include <vector>
#include <iostream>

class ExpandGlob
{
public:
	void expand_glob(const std::filesystem::path& patttern, std::vector<std::filesystem::path>& out) const;
private:
	std::regex glob_to_regex(const std::string& glob) const;
};
