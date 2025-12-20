module;

#include <filesystem>
#include <string>
#include <regex>
#include <vector>
#include <iostream>


export module loc.Filesystem:ExpandGlob;

export class ExpandGlob
{
public:

	void expand_glob(const std::filesystem::path& pattern, std::vector<std::filesystem::path>& out) const
	{
        namespace fs = std::filesystem;

        auto parent = pattern.parent_path().string();
        auto filename = pattern.filename().string();

        bool recursive = false;
        fs::path base_dir = parent.empty() ? "." : parent;

        // detect "**" segment in parent
        if (parent.find("**") != std::string::npos) {
            recursive = true;
            auto pos = parent.find("**");
            if (pos > 0 && (parent[pos - 1] == '/' || parent[pos - 1] == '\\'))
                --pos;
            base_dir = parent.substr(0, pos);
            if (base_dir.empty())
                base_dir = ".";
        }

        auto rex = glob_to_regex(filename);

        if (recursive) {
            for (auto& entry : fs::recursive_directory_iterator(base_dir)) {
                if (!entry.is_regular_file()) continue;
                
                // Skip files in hidden directories (directories starting with '.')
                if (is_in_hidden_directory(entry.path())) continue;
                
                if (std::regex_match(entry.path().filename().string(), rex))
                    out.emplace_back(entry.path());
            }
        }
        else {
            for (auto& entry : fs::directory_iterator(base_dir)) {
                if (!entry.is_regular_file()) continue;
                if (std::regex_match(entry.path().filename().string(), rex))
                    out.emplace_back(entry.path());
            }
        }
	}

private:

	std::regex glob_to_regex(const std::string& glob) const
	{
        std::string re;
        re.reserve(glob.size() * 2);
        for (char c : glob) {
            switch (c) {
            case '*':  re += ".*";  break;
            case '?':  re += '.';   break;
            case '.':  re += "\\."; break;
            default:
                // escape any other non-alphanumeric character
                if (std::isalnum(static_cast<unsigned char>(c)))
                    re += c;
                else {
                    re += '\\';
                    re += c;
                }
            }
        }
        return std::regex{ "^" + re + "$", std::regex::ECMAScript | std::regex::icase };
	}

    bool is_in_hidden_directory(const std::filesystem::path& path) const
    {
        // Check all parent directories to see if any starts with '.'
        for (auto it = path.parent_path(); !it.empty(); it = it.parent_path()) {
            auto dirname = it.filename().string();
            if (!dirname.empty() && dirname[0] == '.') {
                return true;
            }
        }
        return false;
    }
};
