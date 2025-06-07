#pragma once

#include <filesystem>
#include <regex>
#include <string>
#include <vector>
#include <iostream>

class ExpandGlob
{
public:
	void expand_glob(const std::string& pat, std::vector<std::string>& out) const;
private:
	std::regex glob_to_regex(const std::string& glob) const;
};
