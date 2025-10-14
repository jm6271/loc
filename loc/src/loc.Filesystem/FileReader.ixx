module;

#include <string>
#include <vector>
#include <filesystem>
#include <array>
#include <iostream>
#include <fstream>

export module loc.Filesystem:FileReader;

export class FileReader
{
public:

	static void ReadFile(const std::filesystem::path& path, std::vector<std::string>& output)
	{
        std::ifstream file{ path };
        if (!file.is_open()) {
            std::cerr << "Error: unable to open file: " << path << "\n";
            return;
        }

        output.reserve(1000);  // Reserve space for typical file line count

        std::string line;
        line.reserve(256);  // Reserve space for typical line length

        while (std::getline(file, line))
        {
            auto trimmed = trim(line);
            if (!trimmed.empty()) {  // Skip empty lines if desired
                output.emplace_back(trimmed);
            }
        }
	}

private:

    static std::string_view trim(const std::string& s)
    {
		if (s.empty()) return std::string_view(s);

		const char* data = s.data();
		size_t size = s.size();
		size_t start = 0;

		// Find first non-whitespace
		while (start < size && is_whitespace[static_cast<unsigned char>(data[start])]) {
			++start;
		}

		// Find last non-whitespace
		while (size > start && is_whitespace[static_cast<unsigned char>(data[size - 1])]) {
			--size;
		}

		return std::string_view(data + start, size - start);
    }

	static constexpr std::array<bool, 256> is_whitespace = []() {
		std::array<bool, 256> arr{};
		arr[' '] = arr['\t'] = arr['\n'] = arr['\r'] = arr['\v'] = arr['\f'] = true;
		return arr;
		}();
};