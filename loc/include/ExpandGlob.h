#pragma once

#include <filesystem>
#include <string>
#include <regex>
#include <vector>

class ExpandGlob
{
public:

	void expand_glob(const std::filesystem::path& pattern, std::vector<std::filesystem::path>& out) const;

private:

	std::regex glob_to_regex(const std::string& glob) const;

    bool is_in_hidden_directory(const std::filesystem::path& path) const;
};
