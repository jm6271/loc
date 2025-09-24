#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <array>


class FileReader
{
public:
	static void ReadFile(const std::filesystem::path& path, std::vector<std::string>& output);
private:
	static std::string_view trim(const std::string& s);

	static constexpr std::array<bool, 256> is_whitespace = []() {
		std::array<bool, 256> arr{};
		arr[' '] = arr['\t'] = arr['\n'] = arr['\r'] = arr['\v'] = arr['\f'] = true;
		return arr;
	}();
};
